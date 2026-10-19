# Jetson carrier reference — complete component inventory

- **Status:** Extracted Antmicro reference; final ARC population and prices TBD
- **Source:** Antmicro board revision 1.3.4, commit `34f0e7f5fbca441191e20ed55d93cf8614e12bed`; [source lock](../../hardware/orin-carrier/upstream-lock.json), [source schematics](../../hardware/orin-carrier/upstream/jetson-orin-baseboard.kicad_sch)
- **Source licence:** Apache-2.0; [upstream licence](../../hardware/orin-carrier/upstream/LICENSE)
- **Traces to:** E-02, ADR-0025, SYS-07, SYS-08, SYS-24, RSK-01

This table contains **every unique component reference** in the ten pinned KiCad schematic sheets. Multi-unit symbols are counted once per reference: **678 symbol instances → 669 unique references → 121 grouped rows**. References remain explicit so every resistor/capacitor/IC/connector can be found. Grouping requires matching value, manufacturer, MPN, footprint and population flags; different DNP options stay separate.

The source marks **544 references in its BOM**, **77 DNP** and **48 non-purchased test points**; those sum to all 669 references. All 669 have `on_board=yes`. These are reference-design flags, not a final ARC assembly population. Mechanical items can be in the source BOM; DNP devices and PCB test points must not be priced as fitted parts. Fifty references have no source MPN: the 48 test points and two DNP module/reference entries (M3, A2).

> [!IMPORTANT]
> **Reference inventory, not an ARC manufacturing BOM.** ARC still changes the input/PoE arrangement, four CSI connectors, timing/control and module integration under ADR-0025. The imported Altium conversion also needs its recorded connectivity repairs. Do not order all 544 source-BOM parts automatically or add the ARC recommendations on top of replacement reference parts. Final reference designators and population follow the repaired ARC schematics.

## Inventory by source sheet

| Source sheet | Unique references counted here |
| --- | --- |
| [csi.kicad_sch](../../hardware/orin-carrier/upstream/csi.kicad_sch) | 65 |
| [ethernet.kicad_sch](../../hardware/orin-carrier/upstream/ethernet.kicad_sch) | 44 |
| [hdmi.kicad_sch](../../hardware/orin-carrier/upstream/hdmi.kicad_sch) | 58 |
| [jetson-orin-baseboard.kicad_sch](../../hardware/orin-carrier/upstream/jetson-orin-baseboard.kicad_sch) | 4 |
| [m-2.kicad_sch](../../hardware/orin-carrier/upstream/m-2.kicad_sch) | 61 |
| [peripherals.kicad_sch](../../hardware/orin-carrier/upstream/peripherals.kicad_sch) | 93 |
| [som.kicad_sch](../../hardware/orin-carrier/upstream/som.kicad_sch) | 29 |
| [supply.kicad_sch](../../hardware/orin-carrier/upstream/supply.kicad_sch) | 124 |
| [usb.kicad_sch](../../hardware/orin-carrier/upstream/usb.kicad_sch) | 140 |
| [usb3.kicad_sch](../../hardware/orin-carrier/upstream/usb3.kicad_sch) | 51 |
| **Total** | **669** |

The count uses each multi-unit component's first source sheet for the sheet summary; all references are represented in the grouped inventory below. Existing names/MPNs come from the source, with surrounding whitespace removed for display. They are not substituted based on a similar JLC catalogue name.

## Every component group

Unit prices are **TBD** except exact matching SiC431 and FT230X reference MPNs whose retained estimates exist in [pcb-parts-selection.md](pcb-parts-selection.md#jetson-carrier). Those amounts are not live component carts. USD is converted at CAD 1.4271 per USD; no board fabrication, assembly, attrition, MOQ surplus, shipping or 30% is added to these unit figures. No complete reference component-cost total is asserted while the remaining quotes and final ARC population are unresolved.

| All reference designators | Qty in reference | Value / source part | Manufacturer / exact source MPN | Footprint | Reference population status | Unit price basis |
| --- | --- | --- | --- | --- | --- | --- |
| A2 | 1 | Mech_M2_2242_H2.5mm | — / TBD / no source MPN | antmicro-footprints:Mech_M2_2242_H2.5mm | DNP; no purchase in reference | Not purchased |
| BT1 | 1 | Battery_Holder_MS621FE-FL11E | Seiko Instruments / MS621FE-FL11E | antmicro-footprints:MS621FE-FL11E | Populated reference; ARC review pending | TBD quote |
| C1, C2 | 2 | C_Pol_100u_25V_KEMET-T521-D-107 | KEMET / T521D107M025ATE040 | antmicro-footprints:C_Pol_EIA-X | Populated reference; ARC review pending | TBD quote |
| C5, C122, C151, C152 | 4 | C_22u_25V_0805 | Samsung Electro-Mechanics / CL21A226MAYNNNE | antmicro-footprints:C_0805_2012Metric | Populated reference; ARC review pending | TBD quote |
| C6, C7, C8, C25, C26, C27, C29, C31, C33, C36, C37, C40, C41, C42, C43, C44, C45, C48, C50, C52, C53, C54, C55, C57, C58, C60, C61, C62, C63, C64, C65, C67, C68, C69, C70, C71, C72, C73, C75, C78, C86, C87, C88, C89, C90, C91, C92, C96, C97, C100, C101, C102, C104, C111, C113, C118, C119, C121, C124, C125, C130, C131, C132, C133, C135, C138, C139, C140, C141, C143, C144, C157, C158, C159, C163, C164 | 76 | C_100n_0402 | Murata / GRM155R61H104KE14D | antmicro-footprints:C_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| C9, C10, C13, C14, C15, C16, C17, C18, C19, C20, C108, C109, C110, C112 | 14 | C_220n_0402 | YAGEO / CC0402KRX5R6BB224 | antmicro-footprints:C_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| C11, C12, C21, C22, C23, C24, C51, C66, C74 | 9 | C_10u_0603 | Murata / GRM188R61A106KE69D | antmicro-footprints:C_0603_1608Metric | Populated reference; ARC review pending | TBD quote |
| C28, C30, C34, C35, C49, C56, C59, C83, C116, C117, C137 | 11 | C_10u_0402 | YAGEO / CC0402MRX5R5BB106 | antmicro-footprints:C_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| C32, C134 | 2 | C_10u_25V_0603 | TDK / C1608X5R1E106M080AC | antmicro-footprints:C_0603_1608Metric | Populated reference; ARC review pending | TBD quote |
| C47, C93, C94, C95, C98, C99, C105, C106, C107, C114, C115, C123, C142 | 13 | C_1u_0402 | TDK / C1005X6S1A105K050BC | antmicro-footprints:C_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| C76 | 1 | C_100n_100V_0805 | Samsung Electro-Mechanics / CL21B104KCFNNNE | antmicro-footprints:C_0805_2012Metric | Populated reference; ARC review pending | TBD quote |
| C77 | 1 | C_10n_0603 | AVX / 06031C103JAT2A | antmicro-footprints:C_0603_1608Metric | Populated reference; ARC review pending | TBD quote |
| C79, C80, C81, C82 | 4 | C_22u_100V_2220 | TDK / C5750X7S2A226M280KB | antmicro-footprints:C_2220_5650Metric | Populated reference; ARC review pending | TBD quote |
| C84, C85 | 2 | C_47u_16V_1206 | TDK / C3216X5R1C476M160AB | antmicro-footprints:C_1206_3216Metric | Populated reference; ARC review pending | TBD quote |
| C103, C120, C126, C127, C128, C129, C153, C154 | 8 | C_22u_16V_0603 | Samsung Electro-Mechanics / CL10A226MO7JZNC | antmicro-footprints:C_0603_1608Metric_EIA | Populated reference; ARC review pending | TBD quote |
| C136, C148, C155, C156 | 4 | C_4u7_25V_0402 | Murata / GRM155C61E475ME15J | antmicro-footprints:C_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| C145, C146 | 2 | C_47p_0402 | KEMET / C0402C470J5GACTU | antmicro-footprints:C_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| C147, C149, C150 | 3 | C_1u_25V_0402 | Murata / GRM155R61E105KE11J | antmicro-footprints:C_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| C160 | 1 | C_1n_0402 | Murata / GRM1555C1H102JA01D | antmicro-footprints:C_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| C161 | 1 | C_1n_16V_X7R_0402 | KEMET / C0402C102J4RACTU | antmicro-footprints:C_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| D1, D2, D3, D4, D5, D8, D9, D11, D15, D18, D20, D24, D25, D26, D27, D51, D52, D53, D54, D62 | 20 | LED_G_0603_LTST-C190GKT | Lite-On / LTST-C190GKT | antmicro-footprints:LED_0603_1608Metric_G | Populated reference; ARC review pending | TBD quote |
| D6, D7, D28, D29 | 4 | DF2S30FS_SOD | Toshiba / DF2S30FS | antmicro-footprints:SOD-923 | Populated reference; ARC review pending | TBD quote |
| D10, D30, D31, D32, D33 | 5 | ESD9X5.0ST5G | ON Semiconductor / ESD9X5.0ST5G | antmicro-footprints:SOD-923 | Populated reference; ARC review pending | TBD quote |
| D12, D13 | 2 | CD-MMBL110S | Bourns / CD-MMBL110S | antmicro-footprints:DFN3538 | Populated reference; ARC review pending | TBD quote |
| D14 | 1 | PTVS58VS1UR,115 | Nexperia / PTVS58VS1UR,115 | antmicro-footprints:D_SOD-123F | Populated reference; ARC review pending | TBD quote |
| D16, D17, D19, D21, D22, D23, D39, D40, D41, D42, D43, D44, D45, D46, D47, D48, D49, D50, D55, D56, D57, D58, D59, D60, D61 | 25 | TPD1E0B04_XDFN-2 | Texas Instruments / TPD1E0B04DPYR | antmicro-footprints:XDFN-2_1x0.60mm | Populated reference; ARC review pending | TBD quote |
| D34, D35, D36, D37, D38, D63 | 6 | RB521S30T1G | ON Semiconductor / RB521S30T1G | antmicro-footprints:SOD-523 | Populated reference; ARC review pending | TBD quote |
| FB2, FB3, FB4 | 3 | FB_220Z_2A_Murata-BLM21PG_0805 | Murata / BLM21PG221SN1D | antmicro-footprints:FB_0805_2012Metric | Populated reference; ARC review pending | TBD quote |
| H1, H2, H3, H4, H5, H6 | 6 | Spacer_Drill3.7mm_H6.5mm_9774065151R | Würth Elektronik / 9774065151R | antmicro-footprints:Spacer_Drill3.7mm_H6.5mm_9774065151R | Populated reference; ARC review pending | TBD quote |
| J1 | 1 | Bus_M.2_2199230-6 | TE Connectivity / 2199230-6 | antmicro-footprints:Bus_M.2_Socket_Key_E_TE_2199230-6 | Populated reference; ARC review pending | TBD quote |
| J2 | 1 | Bus_M.2_1-2199230-6 | TE Connectivity / 1-2199230-6 | antmicro-footprints:Bus_M.2_Socket_Key_M_TE_1-2199230-6 | Populated reference; ARC review pending | TBD quote |
| J3 | 1 | USB-C_GCT_USB4105-GF-A | GCT / USB4105-GF-A | antmicro-footprints:USB-C_Receptacle_GCT_USB4105-GF-A | Populated reference; ARC review pending | TBD quote |
| J4, J5 | 2 | USB-C_WE_632723300011 | Würth Elektronik / 632723300011 | antmicro-footprints:USB-C_Receptacle_WE_632723300011 | Populated reference; ARC review pending | TBD quote |
| J6 | 1 | Bus_RJ45_5-2337992-8 | TE Connectivity / 5-2337992-8 | antmicro-footprints:RJ45_X-2337992-8_Horizontal | Populated reference; ARC review pending | TBD quote |
| J7, J8 | 2 | Conn_FFC_WE_68715014522 | Würth Elektronik / 68715014522 | antmicro-footprints:Conn_FFC_WE_68715014x22 | Populated reference; ARC review pending | TBD quote |
| J9 | 1 | Tag-Connect_TC2050-IDC-NL_2x5_P1.27mm | Tag Connect / TC2050-IDC-NL | antmicro-footprints:Tag-Connect_TC2050-IDC-NL_2x5_P1.27mm | DNP; no purchase in reference | Not purchased |
| J10 | 1 | Molex_PicoBlade_1x4_0533980471 | Molex / 0533980471 | antmicro-footprints:Conn_Molex_PicoBlade_1x4_0533980471 | Populated reference; ARC review pending | TBD quote |
| J11 | 1 | Socket_Samtec_ERF8_2x40_ERF8-040-05.0-L-DV-K-TR | Samtec / ERF8-040-05.0-L-DV-K-TR | antmicro-footprints:Conn_Socket_Samtec_ERF8_2x40_ERF8-040-05.0-L-DV-K-TR | Populated reference; ARC review pending | TBD quote |
| J12 | 1 | Molex_Nano-Fit_1x2_SMD_1054301202 | Molex / 105430-1202 | antmicro-footprints:Conn_Molex_Nano-Fit_1x2_SMD_1054301202 | Populated reference; ARC review pending | TBD quote |
| J13 | 1 | HDMI-Mini-C_WE_685119136923 | Würth Elektronik / 685119136923 | antmicro-footprints:HDMI-Mini-C_Receptacle_WE_685119136923 | Populated reference; ARC review pending | TBD quote |
| J14 | 1 | JST_SH_1x4_SM04B-SRSS-TB-LF-SN | JST Automotive Connectors / SM04B-SRSS-TB(LF)(SN) | antmicro-footprints:Conn_JST_SH_1x4_SM04B-SRSS-TB-LF-SN | Populated reference; ARC review pending | TBD quote |
| J15 | 1 | Bus_DDR4_SODIMM_CUSTOM_Jetson-Orin-H9.2mm-socket | TE Connectivity / 2309413-1 | antmicro-footprints:TE_2309413 | Populated reference; ARC review pending | TBD quote |
| L1 | 1 | L_560n_17.7A_Bourns-SRP5030CA | Bourns / SRP5030CA-R56M | antmicro-footprints:L_Bourns-SRP5030CA | Populated reference; ARC review pending | TBD quote |
| L4 | 1 | L_1u_20A_Bourns-SRP6050CA | Bourns / SRP6050CA-1R0M | antmicro-footprints:L_Bourns-SRP6050CA | Populated reference; ARC review pending | TBD quote |
| M1 | 1 | Jetson-Orin-NX-Nano | Nvidia / 900-13767-0000-000 | antmicro-footprints:Module_Jetson-Orin-NX-Nano | DNP; no purchase in reference | Not purchased |
| M3 | 1 | Mech_M2_2280_H2.5mm | — / TBD / no source MPN | antmicro-footprints:Mech_M2_2280_H2.5mm | DNP; no purchase in reference | Not purchased |
| Q1, Q2, Q4, Q5, Q7, Q8, Q9, Q10, Q11, Q17, Q19, Q20, Q21, Q22 | 14 | PJE138K | PANJIT / PJE138K_R1_00001 | antmicro-footprints:SOT-523 | Populated reference; ARC review pending | TBD quote |
| Q3, Q12 | 2 | PJE138K | PANJIT / PJE138K_R1_00001 | antmicro-footprints:SOT-523 | DNP; no purchase in reference | Not purchased |
| Q6, Q15, Q16, Q18 | 4 | SSM3J332R | Toshiba / SSM3J332R,LF | antmicro-footprints:SOT-23-3 | Populated reference; ARC review pending | TBD quote |
| Q13, Q14 | 2 | BCM857BS-7-F | Diodes Incorporated / BCM857BS-7-F | antmicro-footprints:SOT-363 | Populated reference; ARC review pending | TBD quote |
| R1, R2, R3, R11, R12, R13, R14, R15, R16, R17, R18, R19, R20, R29, R40, R41, R52, R55, R56, R57, R62, R65, R70, R71, R75, R76, R84, R118, R119, R129, R161, R162, R166, R168, R174, R200, R201, R202, R203, R215, R216, R217, R218, R225, R226, R260, R264, R268, R277, R278 | 50 | R_0R_0402 | Panasonic / ERJ2GE0R00X | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R4, R5, R23, R30, R31, R58, R59, R79, R82, R113, R114, R120, R121, R124, R125, R134, R141, R144, R158, R167, R171, R179, R183, R241, R244, R245, R293 | 27 | R_0R_0402 | Panasonic / ERJ2GE0R00X | antmicro-footprints:R_0402_1005Metric | DNP; no purchase in reference | Not purchased |
| R6, R43, R45, R180, R185, R205, R207, R222, R230, R249, R270, R274, R285 | 13 | R_100k_0402 | Bourns / CR0402-FX-1003GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R7, R8, R9, R10, R80, R81, R105, R106, R130, R164, R165, R246, R262 | 13 | R_4k7_0402 | Bourns / CR0402-FX-4701GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R21, R22, R25, R33, R34, R35, R50, R51, R107, R108, R195, R204 | 12 | R_4x0R_0201_array | Panasonic / EXB-18VR000X | antmicro-footprints:R_Array_4x0201_Panasonic_EXB18V | Populated reference; ARC review pending | TBD quote |
| R24, R42, R44, R46, R47, R49, R66, R67, R74, R77, R83, R99, R116, R131, R135, R136, R137, R138, R148, R160, R163, R181, R188, R189, R211, R219, R232, R238, R242, R250, R251, R254, R255, R259, R269, R275 | 36 | R_10k_0402 | Bourns / CR0402-FX-1002GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R26, R53, R54, R63, R64, R100, R101, R102 | 8 | R_4x330R_0402_array | Panasonic / EXB-28V331JX | antmicro-footprints:R_Array_4x0402 | Populated reference; ARC review pending | TBD quote |
| R27, R28, R32, R37, R38 | 5 | R_47k_0402 | Bourns / CR0402-FX-4702GLF | antmicro-footprints:R_0402_1005Metric | DNP; no purchase in reference | Not purchased |
| R36, R48 | 2 | R_4x2k2_0201_array | Panasonic / EXB-18V222JX | antmicro-footprints:R_Array_4x0201_Panasonic_EXB18V | Populated reference; ARC review pending | TBD quote |
| R39, R68, R69, R72, R73, R132, R133, R147, R151, R152, R186, R187, R191, R223, R231, R236 | 16 | R_200R_0402 | Bourns / CR0402-FX-2000GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R60, R115, R149, R150, R153, R193, R194, R228 | 8 | R_10k_0402 | Bourns / CR0402-FX-1002GLF | antmicro-footprints:R_0402_1005Metric | DNP; no purchase in reference | Not purchased |
| R61, R126, R257, R258, R261, R265, R266 | 7 | R_200k_0402 | Bourns / CR0402-FX-2003GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R78, R104, R139, R227, R294 | 5 | R_2k2_0402 | Bourns / CR0402-FX-2201GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R85, R87, R88, R89, R91, R92, R93, R95, R96, R97 | 10 | R_1k_0402 | Bourns / CR0402-FX-1001GLF | antmicro-footprints:R_0402_1005Metric | DNP; no purchase in reference | Not purchased |
| R86, R90, R94, R98, R111, R155, R156, R184, R256, R267 | 10 | R_1k_0402 | Bourns / CR0402-FX-1001GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R103, R122 | 2 | R_4x0R_0201_array | Panasonic / EXB-18VR000X | antmicro-footprints:R_Array_4x0201_Panasonic_EXB18V | DNP; no purchase in reference | Not purchased |
| R109, R110 | 2 | R_2M_0402 | YAGEO / RC0402FR-072ML | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R112 | 1 | R_4x100R_0201_array | Panasonic / EXB-18V101JX | antmicro-footprints:R_Array_4x0201_Panasonic_EXB18V | DNP; no purchase in reference | Not purchased |
| R117, R127 | 2 | R_27R_0402 | Bourns / CR0402-FX-27R0GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R123 | 1 | R_1k3_0402 | Bourns / CR0402-FX-1301GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R128 | 1 | R_887k_0402 | Panasonic / ERJ-2RKF8873X | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R140, R197, R206, R286, R335, R336 | 6 | R_470R_0402 | Bourns / CR0402-FX-4700GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R142 | 1 | R_49k9_0402 | Bourns / CR0402-FX-4992GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R143, R172, R178, R233, R235, R248, R252, R253, R287 | 9 | R_0R_22.4A_0603 | ROHM Semiconductor / PMR03EZPJ000 | antmicro-footprints:R_0603_1608Metric | Populated reference; ARC review pending | TBD quote |
| R145 | 1 | R_1k2_0402 | Bourns / CR0402-FX-1201GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R146, R154 | 2 | R_24k9_0402 | Bourns / CR0402-FX-2492GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R157, R234, R243 | 3 | R_0R_0603 | Bourns / CR0603-J/-000ELF | antmicro-footprints:R_0603_1608Metric | DNP; no purchase in reference | Not purchased |
| R159, R240 | 2 | R_499k_0402 | Bourns / CR0402-FX-4993GLF | antmicro-footprints:R_0402_1005Metric | DNP; no purchase in reference | Not purchased |
| R169 | 1 | R_2k7_0402 | Bourns / CR0402-FX-2701GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R170 | 1 | R_73k2_0402 | Bourns / CR0402-FX-7322GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R173, R176, R237, R239 | 4 | R_499k_0402 | Bourns / CR0402-FX-4993GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R175, R182 | 2 | R_0R_22.4A_0603 | ROHM Semiconductor / PMR03EZPJ000 | antmicro-footprints:R_0603_1608Metric | DNP; no purchase in reference | Not purchased |
| R177 | 1 | R_45k3_0402 | Bourns / CR0402-FX-4532GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R190 | 1 | R_15k_0402 | Bourns / CR0402-FX-1502GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R192, R229 | 2 | R_100k_0402 | Bourns / CR0402-FX-1003GLF | antmicro-footprints:R_0402_1005Metric | DNP; no purchase in reference | Not purchased |
| R196 | 1 | R_63R4_0603 | Bourns / CR0603-FX-63R4ELF | antmicro-footprints:R_0603_1608Metric | Populated reference; ARC review pending | TBD quote |
| R198, R272 | 2 | R_470k_0402 | Bourns / CR0402-FX-4703GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R199 | 1 | R_470k_0402 | Bourns / CR0402-FX-4703GLF | antmicro-footprints:R_0402_1005Metric | DNP; no purchase in reference | Not purchased |
| R208, R279, R280, R281, R282 | 5 | R_2k2_0402 | Bourns / CR0402-FX-2201GLF | antmicro-footprints:R_0402_1005Metric | DNP; no purchase in reference | Not purchased |
| R209, R210, R212, R213, R214, R220, R221, R224 | 8 | R_499R_0402 | Bourns / CR0402-FX-4990GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R247, R273, R276 | 3 | R_0R_0603 | Bourns / CR0603-J/-000ELF | antmicro-footprints:R_0603_1608Metric | Populated reference; ARC review pending | TBD quote |
| R263 | 1 | R_20k_0402 | Bourns / CR0402-FX-2002GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| R271 | 1 | R_10M_0402 | Bourns / CR0402-FX-1005GLF | antmicro-footprints:R_0402_1005Metric | DNP; no purchase in reference | Not purchased |
| R283, R284 | 2 | R_560R_0402 | Bourns / CR0402-FX-5600GLF | antmicro-footprints:R_0402_1005Metric | Populated reference; ARC review pending | TBD quote |
| S1, S2, S3, S4, S5 | 5 | SW_KMR211NGLFS_SMD | C&K / KMR211NGLFS | antmicro-footprints:SW_KMR211NGLFS_SMD | Populated reference; ARC review pending | TBD quote |
| SP7, SP8 | 2 | Spacer_Drill3.7mm_H2.5mm_9774025151R | Würth Elektronik / 9774025151R | antmicro-footprints:Spacer_Drill3.7mm_H2.5mm_9774025151R | Populated reference; ARC review pending | TBD quote |
| SW1 | 1 | SW_SPDT_PCM12SMTR | C&K / PCM12SMTR | antmicro-footprints:PCM12SMTR | Populated reference; ARC review pending | TBD quote |
| TP1, TP2, TP3, TP4, TP5, TP6, TP7, TP8, TP9, TP10, TP11, TP12, TP13, TP14, TP17, TP18, TP19, TP20, TP21, TP22, TP23, TP24, TP25, TP26, TP27, TP28, TP29, TP30, TP31, TP32, TP33, TP35, TP36, TP37, TP38, TP39, TP40, TP41, TP42, TP43, TP44, TP52, TP53, TP54, TP55, TP56, TP57, TP58 | 48 | TP_0.75mm_SMD | — / TBD / no source MPN | antmicro-footprints:TP_SMD_0.75mm | PCB feature / not in source BOM | Not purchased |
| U1, U40 | 2 | SIC431AED-T1-GE3 | Vishay / SIC431AED-T1-GE3 | antmicro-footprints:MLP44-24L_4x4x0.75mm | Populated reference; ARC review pending | $3.80 CAD; retained $2.66 USD |
| U2, U3 | 2 | 74LVC2G07_SOT457 | Nexperia / 74LVC2G07GV,125 | antmicro-footprints:SC-74-6_1.5x2.9mm_P0.95mm | DNP; no purchase in reference | Not purchased |
| U4, U31, U32, U33, U43, U47 | 6 | NTS0102_XSON | NXP / NTS0102GT | antmicro-footprints:XSON-8_1x1.95mm_P0.5mm | Populated reference; ARC review pending | TBD quote |
| U5 | 1 | FT230X_QFN | FTDI Chip / FT230XQ-R | antmicro-footprints:QFN-16-1EP_4x4mm_P0.65mm | Populated reference; ARC review pending | $9.92 CAD; retained $6.95 USD |
| U6 | 1 | TPS65988DHRSHR | Texas Instruments / TPS65988DHRSHR | antmicro-footprints:IC_TPS65987DDHRSHR | Populated reference; ARC review pending | TBD quote |
| U7 | 1 | W25Q80DV_USON | Winbond / W25Q80DVUXIE TR | antmicro-footprints:USON-8_2x3mm_P0.5mm | Populated reference; ARC review pending | TBD quote |
| U8, U41 | 2 | NCP730BMT330_WSON | ON Semiconductor / NCP730BMT330TBG | antmicro-footprints:WSON-6-1EP_2x2mm_P0.65mm | Populated reference; ARC review pending | TBD quote |
| U9, U12, U18, U24, U25, U27, U28 | 7 | AP22615A_TSOT26 | Diodes Incorporated / AP22615AWU-7 | antmicro-footprints:TSOT23-6 | Populated reference; ARC review pending | TBD quote |
| U10 | 1 | TUSB1046-DCIRNQ | Texas Instruments / TUSB1046-DCIRNQT | antmicro-footprints:WQFN-40_1EP_6x3mm_Pitch0.4mm_EP4.7x2.7mm | Populated reference; ARC review pending | TBD quote |
| U11, U13, U14, U15, U16, U19, U20, U21, U30, U44, U45, U46 | 12 | TPD4E05U06QDQARQ1 | Texas Instruments / TPD4E05U06QDQARQ1 | antmicro-footprints:USON-10_2.5x1mm_P0.5mm | Populated reference; ARC review pending | TBD quote |
| U17 | 1 | HD3SS3220RNH | Texas Instruments / HD3SS3220RNHT | antmicro-footprints:WQFN-30-1EP_4.6x2.6mm_P0.4mm | Populated reference; ARC review pending | TBD quote |
| U22 | 1 | TPS2372-3RGWR | Texas Instruments / TPS2372-3RGWR | antmicro-footprints:VQFN-20_5x5x1mm_P0.65mm | Populated reference; ARC review pending | TBD quote |
| U23 | 1 | ULS-12_5-D48N-C | Murata / ULS-12/5-D48N-C | antmicro-footprints:Conv_Murata_ULS-60W | Populated reference; ARC review pending | TBD quote |
| U26 | 1 | PI4MSD5V9548AZDEX | Diodes Incorporated / PI4MSD5V9548AZDEX | antmicro-footprints:VQFN-24-1EP_3.8x3.8x1.0mm_P0.5mm | Populated reference; ARC review pending | TBD quote |
| U29, U48 | 2 | TPS259474 | Texas Instruments / TPS259474ARPWR | antmicro-footprints:VQFN-HR-10_2x2mm | Populated reference; ARC review pending | TBD quote |
| U34 | 1 | PCAL6408A_VQFN | NXP / PCAL6408ABSHP | antmicro-footprints:VQFN-16-1EP_3x3mm_P0.5mm_EP1.68x1.68mm | Populated reference; ARC review pending | TBD quote |
| U35 | 1 | AP22653_DFN | Diodes Incorporated / AP22653FDZ-7 | antmicro-footprints:DFN-6-1EP_2x2mm_P0.65mm_EP1x1.6mm | Populated reference; ARC review pending | TBD quote |
| U36 | 1 | TS3DV642RUARQ1 | Texas Instruments / TS3DV642RUARQ1 | antmicro-footprints:WQFN-42-1EP_3.5x9mm_P0.5mm | Populated reference; ARC review pending | TBD quote |
| U37 | 1 | 74LVC2G14_SOT457 | Nexperia / 74LVC2G14GV,125 | antmicro-footprints:SOT-23-6 | Populated reference; ARC review pending | TBD quote |
| U38 | 1 | 74LVC1G175_SOT457 | Nexperia / 74LVC1G175GV,125 | antmicro-footprints:SOT-23-6 | Populated reference; ARC review pending | TBD quote |
| U39 | 1 | ACPL-217-500E | Broadcom / ACPL-217-500E | antmicro-footprints:SOIC-4_4.55x2.6mm_P1.27mm | Populated reference; ARC review pending | TBD quote |
| U42 | 1 | TPS7A0518P_SOT23-5 | Texas Instruments / TPS7A0518PDBVT | antmicro-footprints:SOT-23-5 | Populated reference; ARC review pending | TBD quote |
| Y1 | 1 | Oscillator_32.768kHz_ECS_2520MV | ECS Inc. International / ECS-327MVATX-2-CN-TR3 | antmicro-footprints:Oscillator_SMD_ECS_2520MV_2.5x2mm | Populated reference; ARC review pending | TBD quote |

See [the PCB breakdown](pcb-component-breakdown.md#jetson-carrier-e-02) for ARC changes and [total electrical cost](total-electrical-cost.md) for the funding allowance.
