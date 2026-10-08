#!/usr/bin/env bash
# Vendored VESC firmware (ADR-0033): bring upstream vedderb/bldc into
# firmware/vesc/bldc/ and record which upstream commit it's based on in
# firmware/vesc/UPSTREAM. Ordinary commits only, no subtree merges.
#
#   tools/vesc-upstream.sh import <ref>   first import, e.g. release_7_00
#   tools/vesc-upstream.sh update <ref>   bring in upstream's changes up to <ref>
#
# `update` applies upstream's diff between the recorded commit and <ref>
# with a three-way merge, so our changes survive; real conflicts are left
# marked for you to resolve. The result is staged, not committed.
set -euo pipefail

url="${VESC_UPSTREAM_URL:-https://github.com/vedderb/bldc.git}"
dest=firmware/vesc/bldc
record=firmware/vesc/UPSTREAM

# Upstream paths we don't vendor: not used by the firmware build, and
# ~180 MB between them (mostly lispBM's test reports). Skipped on import
# and on every update, so they never creep back in.
exclude=(
  ':(exclude)lispBM/lispBM/test_reports'
  ':(exclude)lispBM/lispBM/repl'
  ':(exclude)lispBM/lispBM/tests'
  ':(exclude)lispBM/lispBM/doc'
  ':(exclude)lispBM/lispBM/videos'
  ':(exclude)lispBM/lispBM/benchmarks'
  ':(exclude)lispBM/lispBM/examples'
  ':(exclude)lispBM/lispBM/mascot'
)

die() { echo "vesc-upstream: $*" >&2; exit 1; }

[ $# -eq 2 ] || die "usage: $0 import|update <ref>"
cmd=$1 ref=$2
cd "$(git rev-parse --show-toplevel)"
[ -z "$(git status --porcelain -- firmware/vesc)" ] || die "firmware/vesc has uncommitted changes"

work="$(mktemp -d)"
trap 'rm -rf -- "$work"' EXIT
git -C "$work" init -q
git -C "$work" remote add upstream "$url"

# fetch one upstream ref or commit, shallowly, and print its commit
fetch() {
  git -C "$work" fetch -q --depth=1 upstream "$1" || die "can't fetch $1 from $url"
  git -C "$work" rev-parse FETCH_HEAD
}

write_record() {
  printf 'repository: %s\nref: %s\ncommit: %s\n' "$url" "$ref" "$1" > "$record"
}

case "$cmd" in
  import)
    [ ! -e "$dest" ] || die "$dest already exists; use update"
    new=$(fetch "$ref")
    mkdir -p "$dest"
    git -C "$work" archive "$new" -- . "${exclude[@]}" | tar -x -C "$dest"
    write_record "$new"
    # -f: upstream's own .gitignore would otherwise skip files upstream tracks
    git add -f -- "$dest" "$record"
    echo "Imported $url $ref ($new) into $dest. Commit it, e.g.:"
    echo "  git commit -m \"chore(vesc): import upstream bldc $ref (${new:0:10})\""
    ;;

  update)
    [ -f "$record" ] || die "no $record; import first"
    base=$(sed -n 's/^commit: //p' "$record")
    [ -n "$base" ] || die "no commit in $record"
    fetch "$base" > /dev/null
    new=$(fetch "$ref")
    [ "$base" != "$new" ] || die "already at $ref ($new)"
    git -C "$work" diff --binary "$base" "$new" -- . "${exclude[@]}" > "$work/upstream.patch"
    # three-way: our copy was imported from $base, so its blobs are in our history
    if git apply --3way --whitespace=nowarn --directory="$dest" "$work/upstream.patch"; then
      status=clean
    else
      status=conflicts
    fi
    write_record "$new"
    # git apply --3way has already staged what it changed, new files included
    git add -- "$record"
    echo "Applied upstream ${base:0:10} -> $ref (${new:0:10}) to $dest: $status."
    if [ "$status" = conflicts ]; then
      echo "Resolve the conflict markers in these files, then git add them:"
      git diff --name-only --diff-filter=U
      exit 2
    fi
    echo "Rebuild and test, then commit, e.g.:"
    echo "  git commit -m \"chore(vesc): update upstream bldc to $ref (${new:0:10})\""
    ;;

  *) die "unknown command $cmd (import or update)" ;;
esac
