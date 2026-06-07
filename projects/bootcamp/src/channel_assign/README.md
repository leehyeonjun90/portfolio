# channel_assign

`channel_assign` is the GUI import mirror for the active CANoe runtime. It is not an independent source tree.

- source of truth for runtime code: `canoe/src/capl/**`
- GUI import mirror: `canoe/cfg/channel_assign/**`
- rule: both trees must stay `1:1` synchronized for active `*.can`

Do not edit files here directly. Edit `canoe/src/capl/**` first, then mirror into this tree and compile after sync.

## Current visible import bank

The current import bank exposes:

- deep runtime anchors: `100`
- shallow placeholder surfaces: `0`
- total visible nodes: `100`

| Domain | Folder | Active nodes |
| --- | --- | --- |
| Chassis | `Chassis/` | `15` |
| Body | `Body/` | `24` |
| Infotainment | `Infotainment/` | `13` |
| Powertrain | `Powertrain/` | `15` |
| ETH Backbone | `ETH_Backbone/` | `9` |
| ADAS | `ADAS/` | `26` |

## Active deep runtime anchors

| Domain | Visible deep nodes |
| --- | --- |
| Chassis | `VCU`, `ESC`, `MDPS`, `ABS`, `EPB`, `TPMS`, `SAS`, `VSM`, `EHB`, `ECS`, `CDC`, `ASM`, `RWS`, `ACU`, `ODS` |
| Body | `BCM`, `DATC`, `SMK`, `AFLS`, `WIP`, `BSEC`, `DOOR_FL`, `DOOR_FR`, `DOOR_RL`, `DOOR_RR`, `SEAT_DRV`, `SEAT_PASS`, `TGM`, `MIR`, `RATC`, `SRF`, `HLM`, `CSM`, `AHLS`, `ADM`, `PTG`, `BIO`, `MSC` |
| Infotainment | `IVI`, `CLU`, `HUD`, `AMP`, `VCS`, `TMU`, `NAV`, `OTA`, `DKEY`, `RSE`, `PGS`, `PAK`, `CPAY` |
| Powertrain | `EMS`, `TCU`, `_4WD`, `BAT_BMS`, `FPCM`, `LVR`, `ISG`, `EOP`, `EWP`, `OBC`, `DCDC`, `MCU`, `INVERTER`, `CPC` |
| ETH Backbone | `CGW`, `V2X`, `SGW`, `IBOX`, `DCM`, `EDR`, `ETHB`, `TEST_SCN`, `TEST_BAS` |
| ADAS | `ADAS`, `SCC`, `LDWS_LKAS`, `FCA`, `BCW`, `LCA`, `SPAS`, `RSPA`, `AVM`, `FCAM`, `FRADAR`, `SRR_FL`, `SRR_FR`, `SRR_RL`, `SRR_RR`, `PUS`, `DMS`, `OMS`, `AEB`, `PKM`, `RPC`, `RRM`, `SPM`, `HWP`, `LDR`, `TRM` |

## GUI import order

1. Open the active CANoe configuration in GUI.
2. Remove duplicate node instances first. Keep a single visible node instance per ECU.
3. Re-attach the five domain CAN DBCs to the matching GUI networks.
4. Import active nodes by domain from `channel_assign/`.
5. Re-apply multibus assignments or extra DB visibility only for the nodes listed below.
6. Compile and save from GUI.

| Network | Import folder | Visible node count |
| --- | --- | --- |
| Chassis | `Chassis/` | `15` |
| Body | `Body/` | `24` |
| Infotainment | `Infotainment/` | `13` |
| Powertrain | `Powertrain/` | `15` |
| ETH_Backbone | `ETH_Backbone/` | `9` |
| ADAS | `ADAS/` | `26` |

## Multibus anchors

These anchors still need extra bus assignments restored in GUI.

| Node | Primary folder | Extra bus context to restore in GUI |
| --- | --- | --- |
| `CGW` | `ETH_Backbone/` | `Chassis`, `Body`, `Infotainment` |
| `TEST_SCN` | `ETH_Backbone/` | `Powertrain`, `Chassis`, `Body`, `Infotainment`, `ADAS` |

`TEST_BAS` is intentionally **not** a multibus anchor. It stays on `ETH_Backbone/` as a narrow validation result aggregator.

Formal rationale:

- [multibus-policy.md](../../docs/contracts/multibus-policy.md)

## Fresh cfg rebuild checklist

If the active GUI configuration was rebuilt from scratch or old DBC assignments were changed, restore the database attachment first.

| GUI network / folder | Attach this DBC |
| --- | --- |
| `Powertrain/` | `canoe/databases/powertrain_can.dbc` |
| `Chassis/` | `canoe/databases/chassis_can.dbc` |
| `Body/` | `canoe/databases/body_can.dbc` |
| `Infotainment/` | `canoe/databases/infotainment_can.dbc` |
| `ADAS/` | `canoe/databases/adas_can.dbc` |

`ETH_Backbone/` uses UDP helper code and does not require a CAN DBC for primary runtime semantics.

Do not keep stale legacy DBC attachments in the same GUI config. Re-attach the five current domain CAN DBCs first, then restore only the documented foreign CAN visibility.

## Cross-domain visibility restore candidates

The nodes below reference messages outside their primary domain DBC. If they are imported into a fresh cfg as a single node instance, restore additional bus / DB visibility in GUI instead of duplicating the node.

| Node | Primary folder | Also needs visibility to |
| --- | --- | --- |
| `IVI` | `Infotainment/` | `ETH_Backbone` (`ethNavContextMsg`) |
| `PGS` | `Infotainment/` | `ADAS` (`frmParkUltrasonicStateMsg`) |
| `AFLS` | `Body/` | `Chassis` (`frmSteeringAngleMsg`) |
| `DATC` | `Body/` | `Infotainment` (`frmTmuServiceStateMsg`) |
| `ACU` | `Chassis/` | `Body` (`frmSeatBeltStateMsg`) |
| `ODS` | `Chassis/` | `Body` (`frmSeatBeltStateMsg`, `frmSeatStateMsg`) |
| `VCU` | `Chassis/` | `Powertrain` |
| `SCC` | `ADAS/` | `Chassis` (`frmVehicleStateCanMsg`), `Powertrain` (`frmSccDiagReqMsg`) |
| `HWP` | `ADAS/` | `Powertrain` (`frmCruiseStateMsg`) |

Nodes not listed above should compile on their primary domain DBC if the five current domain CAN DBCs were attached correctly.

## Node intent

### Chassis
- `VCU.can` ? propulsion / accel command owner
- `ESC.can` ? brake / stability owner
- `MDPS.can` ? steering owner
- `ABS.can`, `EPB.can`, `TPMS.can`, `SAS.can`, `VSM.can`, `EHB.can`, `ECS.can`, `CDC.can`, `ASM.can`, `RWS.can` ? chassis/safety runtime anchors
- `TEST_BAS.can` ? backbone-side validation baseline aggregation

### Body
- `BCM.can` ? body output owner after hazard/window/ambient/driver-state absorption
- `DATC.can`, `SMK.can`, `AFLS.can`, `WIP.can`, `BSEC.can`, `DOOR_FL.can`, `DOOR_FR.can`, `DOOR_RL.can`, `DOOR_RR.can`, `SEAT_DRV.can`, `SEAT_PASS.can`, `TGM.can`, `MIR.can`, `RATC.can`, `SRF.can`, `HLM.can`, `CSM.can`, `AHLS.can`, `ADM.can`, `PTG.can`, `BIO.can` ? comfort/security runtime anchors

### Infotainment
- `IVI.can` ? IVI display/connectivity/diagnostic owner after navigation owner split
- `NAV.can`, `OTA.can`, `DKEY.can`, `RSE.can`, `PGS.can`, `PAK.can`, `CPAY.can` ? infotainment service/runtime anchors
- `CLU.can` ? cluster display / HMI owner
- `HUD.can`, `AMP.can`, `VCS.can`, `TMU.can` ? HMI/connectivity runtime anchors

### Powertrain
- `EMS.can` ? engine management runtime
- `TCU.can` ? transmission control runtime
- `_4WD.can`, `BAT_BMS.can`, `FPCM.can`, `LVR.can`, `ISG.can`, `EOP.can`, `EWP.can`, `OBC.can`, `DCDC.can`, `MCU.can`, `INVERTER.can`, `CPC.can` ? driveline and power electronics runtime anchors

### ETH Backbone
- `CGW.can` ? cross-domain boundary, fail-safe, and gateway authority
- `V2X.can` ? V2X emergency input / arbitration ingress
- `SGW.can`, `IBOX.can`, `DCM.can`, `EDR.can`, `ETHB.can` ? security/infra/connectivity anchors
- `TEST_SCN.can` ? validation scenario orchestrator

### ADAS
- `ADAS.can` ? integrated risk, warning, and assist decision runtime
- `SCC.can`, `LDWS_LKAS.can`, `FCA.can`, `BCW.can`, `LCA.can`, `SPAS.can`, `RSPA.can`, `AVM.can`, `FCAM.can`, `FRADAR.can`, `SRR_FL.can`, `SRR_FR.can`, `SRR_RL.can`, `SRR_RR.can`, `PUS.can`, `DMS.can`, `OMS.can`, `AEB.can`, `PKM.can`, `RPC.can`, `RRM.can`, `SPM.can`, `HWP.can`, `LDR.can`, `TRM.can` ? ADAS feature/sensor/runtime anchors

## Placeholder note

The original placeholder bank is fully retired from the active branch. Historical placeholder and v1 assets are preserved in archive branches, not in the working tree.

## Validation

- `python scripts/gates/text_integrity_gate.py`
- `python scripts/gates/cfg_hygiene_gate.py`

`check_capl_sync.py` still needs the new ECU inventory update outside Dev1 scope.
Local runtime verification result for the current visible bank:

- `src/capl` visible files: `100`
- `cfg/channel_assign` visible files: `100`
- name diff: `0`
- content diff: `0`

