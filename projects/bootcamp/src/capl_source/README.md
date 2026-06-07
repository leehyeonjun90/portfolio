# src/capl

`src/capl` is the source of truth for active CAPL runtime code.

- edit files here first
- mirror the same change into `canoe/cfg/channel_assign/**` immediately after each edit
- do not treat `canoe/cfg/channel_assign/**` as an independent code tree
- compile after mirror sync

## Role Split

- `common/` shared include files
- `ecu/` active runtime ECU nodes
- `input/` scenario ingress nodes
- `logic/` shared decision and coordination logic
- `output/` output owner nodes
