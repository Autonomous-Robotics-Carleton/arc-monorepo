"""The replay backend's topics against ICD-ros2-msgs: it plays back every
topic the car publishes (Side A's measurements and state) and nothing else,
and listens on every command topic, the way the car backend does. Fails when
the ICD or the backend changes without the other.
"""

from pathlib import Path
import re

PACKAGE = Path(__file__).resolve().parents[1]
REPO = PACKAGE.parents[2]
BACKEND = (PACKAGE / 'src/backend.cpp').read_text()
ICD = (REPO / 'systems/icd/ros2-msgs.md').read_text()

# Side A sections whose topics the car backend publishes: replay plays these
PLAYED = ('From the sync link', "From the Orin's drivers", 'Ground velocity', "The car's state")
# Everything else the ICD names: never played from a log
NOT_PLAYED = ('Time and transforms', 'Commands: Side B → Side A', 'One publisher per command',
              'Platform topics experiments use', 'Choosing who drives')


def car_topics_regex():
    m = re.search(r'kCarTopics\[\]\s*=\s*((?:\s*"[^"]*")+);', BACKEND)
    assert m, 'kCarTopics not found in backend.cpp'
    return re.compile(''.join(re.findall(r'"([^"]*)"', m.group(1))))


def car_topic_names():
    m = re.search(r'kCarTopicNames = \{(.*?)\};', BACKEND, re.S)
    assert m, 'kCarTopicNames not found in backend.cpp'
    return re.findall(r'"([^"]*)"', m.group(1))


def expand(name):
    """`/a/{x,y}` -> `/a/x`, `/a/y` (the ICD's shorthand)."""
    m = re.search(r'\{([^}]*)\}', name)
    if not m:
        return [name]
    return [n for part in m.group(1).split(',')
            for n in expand(name[:m.start()] + part.strip() + name[m.end():])]


def topics(section):
    """The topics in the first column of the tables under `section`."""
    m = re.search(rf'^#+ {re.escape(section)}[^\n]*\n(.*?)(?=^#+ )', ICD, re.M | re.S)
    assert m, f'ICD-ros2-msgs has no section "{section}": update this test'
    found = []
    for row in re.findall(r'^\| (.*?) \|', m.group(1), re.M):
        last = None
        for name in re.findall(r'`([^`]+)`', row):
            if name.startswith('…/') and last:
                # `…/camera_info` after `/a/{x,y}/image_raw`: `/a/{x,y}/camera_info`
                name = last.rsplit('/', 1)[0] + name[1:]
            if name.startswith('/'):
                found += expand(name)
                last = name
    return found


def test_every_car_topic_is_played():
    regex = car_topics_regex()
    played = [t for s in PLAYED for t in topics(s)]
    assert len(played) >= 30, played  # the tables were read
    missing = [t for t in played if not regex.fullmatch(t)]
    assert not missing, f'in the ICD but not replayed: {missing}'


def test_nothing_else_is_played():
    regex = car_topics_regex()
    others = [t for s in NOT_PLAYED for t in topics(s)]
    assert '/car/cmd/drive' in others and '/clock' in others
    played = [t for t in others if regex.fullmatch(t)]
    assert not played, f'replayed but not the car\'s to publish: {played}'


def test_the_named_car_topics_are_the_icds():
    """The list the backend reports missing topics from: the ICD's, each played."""
    names = car_topic_names()
    played = [t for s in PLAYED for t in topics(s)]
    assert len(names) == len(set(names)), 'a topic is named twice'
    assert set(names) == set(played), {'not in the ICD': set(names) - set(played),
                                       'not named': set(played) - set(names)}
    regex = car_topics_regex()
    assert all(regex.fullmatch(t) for t in names)


def test_every_command_topic_is_listened_on():
    listened = {'/' + t for t in re.findall(r'listen<[\w:]+>\("([^"]+)"', BACKEND)}
    commands = set(topics('Commands: Side B → Side A'))
    assert commands and listened == commands, (listened, commands)


def test_command_qos_matches_the_icd():
    """Commands volatile, the envelope (State) transient local, as in the ICD's rows."""
    rows = re.search(r'^## Commands: Side B → Side A\n(.*?)(?=^#+ )', ICD, re.M | re.S).group(1)
    for topic, profile in re.findall(r'^\| `(/car/cmd/\w+)` \| `[^`]+` \| (\w+) \|', rows, re.M):
        call = re.search(rf'listen<[\w:]+>\("{topic[1:]}", (\w+)\)', BACKEND)
        assert call, topic
        assert call.group(1) == {'Commands': 'kCommands', 'State': 'kState'}[profile], topic
