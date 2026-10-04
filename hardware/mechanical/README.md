# hardware/mechanical

Released mechanical geometry, one folder per release (e.g. `mech-r1/`):

- `car.step`: the full assembly
- `<part>.step`: each part we make (machined or printed)
- `drawings/<part>.pdf`: dimensioned drawings for machined parts, with tolerances, material and finish (what the CNC service quotes from)

The live CAD is in the Fusion 360 team hub. This folder only gets exports at a release, after the fab gate. The process is in [`systems/mechanical/cad-workflow.md`](../../systems/mechanical/cad-workflow.md).

STEP and PDF files go through git LFS (see `.gitattributes`). Run `git lfs install` once before committing any.
