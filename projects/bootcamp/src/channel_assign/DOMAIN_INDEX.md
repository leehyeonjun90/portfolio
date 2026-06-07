# V2 Domain CAPL Index

This file documents the visible runtime and placeholder CAPL inventory after wrapper absorption and OEM abbreviation normalization.

## Runtime SSoT
- v2 runtime cfg: `canoe/cfg/CAN_v2_topology_wip.cfg`
- runtime source: `canoe/src/capl/**`
- GUI import mirror: `canoe/cfg/channel_assign/**`
- shared sysvars: `canoe/project/sysvars/project.sysvars`

## Visible domain inventory

### ETH_Backbone
- `CGW.can`
- `V2X.can`
- `TEST_SCN.can`
- `TEST_BAS.can`
- `EXT_DIAG.can`
- `ETHB.can`
- `DCM.can`
- `IBOX.can`
- `SGW.can`
- `EDR.can`

### ADAS
- `ADAS.can`
- `SCC.can`
- `LDWS_LKAS.can`
- `FCA.can`
- `BCW.can`
- `LCA.can`
- `SPAS.can`
- `RSPA.can`
- `AVM.can`
- `FCAM.can`
- `FRADAR.can`
- `SRR_FL.can`
- `SRR_FR.can`
- `SRR_RL.can`
- `SRR_RR.can`
- `PUS.can`
- `DMS.can`
- `OMS.can`
- `AEB.can`
- `PKM.can`
- `RPC.can`
- `LDR.can`
- `RRM.can`
- `SPM.can`
- `HWP.can`
- `TRM.can`

### Infotainment
- `IVI.can`
- `CLU.can`
- `HUD.can`
- `TMU.can`
- `AMP.can`
- `PGS.can`
- `NAV.can`
- `VCS.can`
- `RSE.can`
- `DKEY.can`
- `CPAY.can`
- `PAK.can`
- `OTA.can`

### Body
- `BCM.can`
- `DATC.can`
- `SMK.can`
- `AFLS.can`
- `AHLS.can`
- `WIP.can`
- `SRF.can`
- `DOOR_FL.can`
- `DOOR_FR.can`
- `DOOR_RL.can`
- `DOOR_RR.can`
- `TGM.can`
- `SEAT_DRV.can`
- `SEAT_PASS.can`
- `MIR.can`
- `BSEC.can`
- `HLM.can`
- `ADM.can`
- `PTG.can`
- `MSC.can`
- `RATC.can`
- `CSM.can`
- `BIO.can`

### Powertrain
- `EMS.can`
- `TCU.can`
- `_4WD.can`
- `BAT_BMS.can`
- `FPCM.can`
- `LVR.can`
- `ISG.can`
- `EOP.can`
- `EWP.can`
- `OBC.can`
- `DCDC.can`
- `MCU.can`
- `INVERTER.can`
- `CPC.can`

### Chassis
- `VCU.can`
- `ESC.can`
- `MDPS.can`
- `ABS.can`
- `EPB.can`
- `TPMS.can`
- `SAS.can`
- `ECS.can`
- `ACU.can`
- `ODS.can`
- `VSM.can`
- `EHB.can`
- `CDC.can`
- `ASM.can`
- `RWS.can`

## Validation summary
- deep runtime anchors: `13`
- validation-only nodes: `2`
- placeholder surface nodes: `87`
- total visible nodes: `100`

