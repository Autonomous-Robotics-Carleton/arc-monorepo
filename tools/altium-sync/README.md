# Altium 365 source snapshots in the monorepo

The four active ARC PCB projects are edited and versioned in Altium 365. This tool imports their local source files into the board folders in this monorepo. A normal Git commit and push then publishes those files to GitHub. The import never stages, commits, pushes, fetches, changes remotes, or uploads to Altium 365.

This is a **one-way snapshot workflow**. Altium 365's project repository and the ARC monorepo are separate repositories; do not copy a managed checkout's `.git` folder into the monorepo. Edit the managed Altium project, then import it here. Unsaved editor changes and cloud commits that have not been downloaded into the local checkout are not visible to this tool.

**Online updates do not automatically enter the monorepo.** The configured sequence is **Save to Server from Altium Designer → Update this PC's desktop checkout → run the Nx sync command → review, commit and push to GitHub**. The sync command does not poll Altium 365, and `git push` alone sends only changes that have already been committed.

## Configure once per computer

Use Node 22.18 or newer, as for the other TypeScript repo scripts. Copy [config.example.json](config.example.json) to `.altium-sync/config.json` at the repo root, and set the four paths to the actual Altium checkout directories. `.altium-sync/` is ignored by Git and contains local paths, backups and optional downloaded snapshots. Paths can be absolute, or relative to the config file. A config can include a subset of the four projects when importing only those boards.

The example uses Altium's `C:/Users/Public/Documents/Altium/` checkout location on the configured ARC computer. Another member should use their own checkout paths. Each source folder must contain exactly one root `.PrjPcb` and all its referenced schematic/PCB documents. A cloud display-name change may leave the local project named `Main Board.PrjPcb`; the importer exports it under the stable `ARC-Motherboard.PrjPcb` name without changing its contents or document references. A matching `.PrjPcbVariants` sidecar is preserved under the same stable basename; it contains actual alternate-part symbol data, unlike the generated `.PrjPcbStructure` cache.

On the configured ARC computer, the clean ToF checkout is `C:/Users/Public/Documents/Altium/ARC-Sensor-ToF_1`. The directory without `_1` belongs to the replaced sample-template project and must not be imported. The ignored local config selects the clean directory; the portable example uses the unsuffixed name for a fresh computer.

When the source has a nonempty `ManagedProjectGUID`, it must match the configured cloud project above; stale or swapped checkouts are rejected. Unlinked source snapshots with a blank or absent GUID are allowed, as used by the initial native upload packages. For those snapshots, confirm the board and its contents when choosing the config mapping: a blank GUID cannot prove cloud identity or freshness.

| Config key | Monorepo destination | Cloud project |
| --- | --- | --- |
| `motherboard` | `hardware/motherboard/altium/` | [ARC-Motherboard](https://americas-carleton-university-autonomous-robotics-carle.365.altium.com/designs/435E9B62-9567-4537-918E-7E4772E07F96#design) |
| `angle` | `hardware/sensor-boards/angle/altium/` | [ARC-Sensor-Angle-AS5047P](https://americas-carleton-university-autonomous-robotics-carle.365.altium.com/designs/B0EECA79-5052-4095-B203-15090846669F#design) |
| `imu` | `hardware/sensor-boards/imu/altium/` | [ARC-Sensor-IMU-ISM330DHCX](https://americas-carleton-university-autonomous-robotics-carle.365.altium.com/designs/2A72933B-BA15-4B7E-AB7B-3331F4D6727C#design) |
| `tof` | `hardware/sensor-boards/tof/altium/` | [ARC-Sensor-ToF](https://americas-carleton-university-autonomous-robotics-carle.365.altium.com/designs/1CE3083D-9559-41D0-BFD7-DBE654C9101D#design) |

## Before a GitHub commit

1. The designer saves their work and uses **Save to Server**. On the computer importing into this monorepo, open the ARC workspace project in Altium Designer and **Update** its local checkout to obtain the saved cloud changes. Reconcile conflicts in Altium first; pause design edits while importing.
2. Preview the import from the repo root:

   ```bash
   npx nx run altium-sync:sync
   ```

3. Apply the reviewed import:

   ```bash
   npx nx run altium-sync:sync --apply
   ```

4. Review `git status` and the hardware changes. Commit and push through the repo's usual workflow when ready. No hook or background job performs these steps automatically.

The first import can encounter a different pre-existing scaffold. Review the preview with `--adopt-existing`, then apply with `--apply --adopt-existing` to adopt those files. Every overwritten file gets a local recovery copy. This flag only permits first-import adoption; it does not override edits protected by an existing sync manifest. Unknown files are never deleted on the first import.

`--dry-run` explicitly requests the default preview. `--config PATH` selects another local mapping. `--require-clean` additionally requires a clean Git source checkout and records its `HEAD` revision; it needs read access to that checkout's `.git` metadata. It is optional because some Altium desktop checkout ACLs restrict command-line Git access. Without it, the tool imports the checked files and records their SHA-256 hashes, without claiming they represent the latest cloud revision.

Direct commands are equivalent when Nx is unavailable:

```bash
node tools/altium-sync/sync.mts --dry-run
node tools/altium-sync/sync.mts --apply
node --test tools/altium-sync/sync.test.mts
```

Node 22.11–22.17 needs `--experimental-strip-types` before the script name (and before `--test` for tests). The Nx targets include that flag for compatibility with the configured computer's Node 22.11. The repo's supported 22.18+ also runs these files directly.

## Browser download alternative

From each Altium 365 project's Design export menu, download the source snapshot, or use **History → Download Sources** for a specific saved revision. Extract each ZIP into its own directory under `.altium-sync/sources/`, then point the config at the directory containing the `.PrjPcb`. Run the same preview/apply steps. ZIP extraction is deliberately separate from the importer; do not point it at an arbitrary whole Downloads folder.

Altium documents [source snapshots from project history](https://www.altium.com/documentation/altium-365/project-history) and [managed versus external version control](https://www.altium.com/documentation/altium-designer/making-existing-project-available-in-workspace). This workflow needs no Workspace API token, stored password or GitHub token. It does not automatically poll the cloud or create a bidirectional Git mirror.

## File handling and recovery

- All project source assets are copied, including libraries, images and mechanical models. The importer validates the project references before writing. `.git`, hidden files, histories, previews, output/log folders, backups, locks, temporary files and unresolved LFS pointers are excluded or rejected. Keys/credential files and symlinks/junctions are rejected.
- Repo `README.md`, `.gitattributes` and `altium-sync.json` are protected. The manifest records the cloud project URL and imported file hashes. Only paths owned by the previous manifest can be removed when they disappear from the new source snapshot.
- A changed or locally deleted destination file causes the import to stop unless it already matches the new source exactly. Reconcile or preserve that work before retrying; there is no force-overwrite option for previously managed edits.
- All projects are planned before the first CAD write. Apply rechecks source/destination hashes and keeps recovery files under `.altium-sync/backups/<run>/<project>/`, with `transaction.json` listing affected destinations. If a write fails, rollback includes that possibly partial write and attempts every restoration even if another restoration fails. Any failed restorations are reported together with the original error and recovery-copy location. After a process interruption, use those backups to restore the listed files, and check the local lock before removing a stale `.altium-sync/lock`.

Circuit capture, layout, verification and fabrication approval remain the board owners' work. Synchronizing a file does not approve its engineering content.
