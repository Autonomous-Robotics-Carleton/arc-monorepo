import { readFileSync } from 'node:fs';
import { resolve } from 'node:path';

// Message catalogues rendered from the machine-readable ICDs when the site is
// built, so they can't drift from the schemas the code is generated from.

const repoRoot = resolve(process.cwd(), '../..'); // builds run in apps/docs
const read = (path: string) => readFileSync(resolve(repoRoot, path), 'utf8');

const unescape = (text: string) =>
  text
    .replace(/&lt;/g, '<')
    .replace(/&gt;/g, '>')
    .replace(/&quot;/g, '"')
    .replace(/&apos;/g, "'")
    .replace(/&amp;/g, '&')
    .replace(/\s+/g, ' ')
    .trim();

const attrs = (text: string) =>
  Object.fromEntries([...text.matchAll(/(\w+)="([^"]*)"/g)].map(([, key, value]) => [key, unescape(value)]));

const descriptionOf = (body: string) => unescape(body.match(/<description>([\s\S]*?)<\/description>/)?.[1] ?? '');

// MAVLink 2 wire size of each field type; arrays are written type[n]
const typeSize: Record<string, number> = {
  uint64_t: 8, int64_t: 8, double: 8,
  uint32_t: 4, int32_t: 4, float: 4,
  uint16_t: 2, int16_t: 2,
  uint8_t: 1, int8_t: 1, char: 1,
};

function fieldSize(type: string): number {
  const [, base, count] = type.match(/^(\w+)(?:\[(\d+)\])?$/) ?? [];
  return (typeSize[base] ?? 0) * Number(count ?? 1);
}

interface MavlinkField { type: string; name: string; units?: string; enum?: string; description: string }
interface MavlinkMessage { id: string; name: string; group: string; description: string; fields: MavlinkField[] }
interface MavlinkEnum { name: string; bitmask: boolean; description: string; entries: { value: string; name: string; description: string }[] }

function parseMavlink(xml: string) {
  const version = xml.match(/<version>(\d+)<\/version>/)?.[1];

  const enums: MavlinkEnum[] = [...xml.matchAll(/<enum ([^>]*)>([\s\S]*?)<\/enum>/g)].map(([, head, body]) => {
    const { name, bitmask } = attrs(head);
    return {
      name,
      bitmask: bitmask === 'true',
      description: descriptionOf(body.replace(/<entry[\s\S]*$/, '')),
      entries: [...body.matchAll(/<entry ([^>]*?)(?:\/>|>([\s\S]*?)<\/entry>)/g)].map(([, entryHead, entryBody]) => ({
        value: attrs(entryHead).value,
        name: attrs(entryHead).name,
        description: descriptionOf(entryBody ?? ''),
      })),
    };
  });

  // messages are grouped by the comments between them (e.g. "Sync MCU → Orin")
  const messages: MavlinkMessage[] = [];
  let group = '';
  const block = xml.match(/<messages>([\s\S]*)<\/messages>/)?.[1] ?? '';
  for (const [, comment, head, body] of block.matchAll(/<!--([\s\S]*?)-->|<message ([^>]*)>([\s\S]*?)<\/message>/g)) {
    if (comment !== undefined) {
      group = unescape(comment);
      continue;
    }
    const { id, name } = attrs(head);
    const fields = [...body.matchAll(/<field ([^>]*)>([\s\S]*?)<\/field>/g)].map(([, fieldHead, text]) => {
      const field = attrs(fieldHead);
      return { type: field.type, name: field.name, units: field.units, enum: field.enum, description: unescape(text) };
    });
    messages.push({ id, name, group, description: descriptionOf(body), fields });
  }
  return { version, enums, messages };
}

const anchor = (name: string) => name.toLowerCase().replace(/_/g, '-');

/** The sync-link message set (systems/icd/sync-link.xml): MAVLink 2, ADR-0031. */
export function SyncLinkMessages() {
  const { version, enums, messages } = parseMavlink(read('systems/icd/sync-link.xml'));
  const groups = [...new Set(messages.map((message) => message.group))];

  return (
    <>
      <p>
        Schema version {version}. Message IDs, fields and their order on the wire come from the schema; MAVLink 2 adds a
        10-byte header and a 2-byte CRC to each payload.
      </p>
      {groups.map((group) => (
        <section key={group}>
          <h3 id={anchor(group)}>{group || 'Messages'}</h3>
          {messages
            .filter((message) => message.group === group)
            .map((message) => {
              const payload = message.fields.reduce((sum, field) => sum + fieldSize(field.type), 0);
              return (
                <section key={message.id}>
                  <h4 id={anchor(message.name)}>
                    <code>{message.name}</code> ({message.id})
                  </h4>
                  <p>
                    {message.description} Payload {payload} bytes, {payload + 12} on the wire.
                  </p>
                  <table>
                    <thead>
                      <tr>
                        <th>Field</th>
                        <th>Type</th>
                        <th>Units</th>
                        <th>Description</th>
                      </tr>
                    </thead>
                    <tbody>
                      {message.fields.map((field) => (
                        <tr key={field.name}>
                          <td>
                            <code>{field.name}</code>
                          </td>
                          <td>
                            <code>{field.type}</code>
                          </td>
                          <td>{field.units}</td>
                          <td>
                            {field.description}
                            {field.enum && (
                              <>
                                {' '}
                                (<a href={`#${anchor(field.enum)}`}>
                                  <code>{field.enum}</code>
                                </a>
                                )
                              </>
                            )}
                          </td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                </section>
              );
            })}
        </section>
      ))}
      <h3 id="enums">Enums</h3>
      {enums.map((item) => (
        <section key={item.name}>
          <h4 id={anchor(item.name)}>
            <code>{item.name}</code>
            {item.bitmask && ' (bitmask)'}
          </h4>
          <p>{item.description}</p>
          <table>
            <thead>
              <tr>
                <th>Value</th>
                <th>Name</th>
                <th>Description</th>
              </tr>
            </thead>
            <tbody>
              {item.entries.map((entry) => (
                <tr key={entry.name}>
                  <td>{entry.value}</td>
                  <td>
                    <code>{entry.name}</code>
                  </td>
                  <td>{entry.description}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </section>
      ))}
    </>
  );
}

interface DbcSignal { name: string; start: string; bits: string; byteOrder: string; signed: boolean; scale: string; offset: string; min: string; max: string; unit: string }
interface DbcMessage { id: number; name: string; dlc: string; sender: string; signals: DbcSignal[]; comment: string }

function parseDbc(dbc: string) {
  const comments = new Map(
    [...dbc.matchAll(/^CM_ BO_ (\d+) "([^"]*)";/gm)].map(([, id, text]) => [Number(id), text]),
  );
  const messages: DbcMessage[] = [];
  for (const [, id, name, dlc, sender, body] of dbc.matchAll(/^BO_ (\d+) (\w+): (\d+) (\w+)\n((?: SG_ .*\n?)*)/gm)) {
    const signals = [...body.matchAll(/ SG_ (\w+) : (\d+)\|(\d+)@([01])([+-]) \(([^,]+),([^)]+)\) \[([^|]*)\|([^\]]*)\] "([^"]*)"/g)].map(
      ([, signal, start, bits, order, sign, scale, offset, min, max, unit]) => ({
        name: signal, start, bits, byteOrder: order === '0' ? 'big-endian' : 'little-endian',
        signed: sign === '-', scale, offset, min, max, unit,
      }),
    );
    messages.push({ id: Number(id), name, dlc, sender, signals, comment: comments.get(Number(id)) ?? '' });
  }
  const database = dbc.match(/^CM_ "([^"]*)";/m)?.[1] ?? '';
  return { database, messages };
}

const corners = /_(FL|FR|RL|RR)$/;
// in a DBC file, bit 31 of the ID marks an extended (29-bit) frame
const canId = (id: number) => `0x${(id & 0x1fffffff).toString(16).toUpperCase().padStart(8, '0')}`;

/** The command-bus frames (systems/icd/can-command.dbc): stock VESC frames, ADR-0009. */
export function CanCommandMessages() {
  const { database, messages } = parseDbc(read('systems/icd/can-command.dbc'));
  // one entry per frame type; the four corners differ only in ID and node
  const kinds = [...new Set(messages.map((message) => message.name.replace(corners, '')))];

  return (
    <>
      <p>{database}</p>
      {kinds.map((kind) => {
        const instances = messages.filter((message) => message.name.replace(corners, '') === kind);
        const first = instances[0];
        return (
          <section key={kind}>
            <h4 id={anchor(kind)}>
              <code>{kind}</code>
            </h4>
            <p>
              {first.comment} {first.dlc} bytes. Extended IDs:{' '}
              {instances.map((message, i) => (
                <span key={message.name}>
                  {i > 0 && ', '}
                  {message.name.match(corners)?.[1] ?? message.name} <code>{canId(message.id)}</code>
                </span>
              ))}
              .
            </p>
            <table>
              <thead>
                <tr>
                  <th>Signal</th>
                  <th>Start bit</th>
                  <th>Length</th>
                  <th>Encoding</th>
                  <th>Scale</th>
                  <th>Range</th>
                  <th>Unit</th>
                </tr>
              </thead>
              <tbody>
                {first.signals.map((signal) => (
                  <tr key={signal.name}>
                    <td>
                      <code>{signal.name}</code>
                    </td>
                    <td>{signal.start}</td>
                    <td>{signal.bits}</td>
                    <td>
                      {signal.signed ? 'signed' : 'unsigned'}, {signal.byteOrder}
                    </td>
                    <td>
                      ×{signal.scale}
                      {signal.offset !== '0' && ` + ${signal.offset}`}
                    </td>
                    <td>
                      {signal.min} to {signal.max}
                    </td>
                    <td>{signal.unit}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </section>
        );
      })}
    </>
  );
}
