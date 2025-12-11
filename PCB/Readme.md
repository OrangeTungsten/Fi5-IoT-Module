This document provides a complete overview of the device’s hardware interface and explains the steps required for proper configuration. It includes annotated images showing the pinout, detailed descriptions of each connector and button, as well as the full procedure for the initial setup and operation.

## Pinout
On the image below, pinout of MainPCB is given. 

<img width="712" height="520"  alt="Device config 1" src="https://github.com/user-attachments/assets/a3d7cf66-c838-431d-9103-a9e0c8c127a6" />


Device features ports:
- **BTN_PORT** (X1) - used for connecting four configuration pushbuttons (ButtonsPCB peripheral);
- **3DP_PORT** (X2) - used for communication with 3D printer
- **PC_PORT** (X3) - used for connecting computer via UART to USB converter
- **DISP_PORT** (X4) - used for the E-ink display pheripheral

Flashing firmware and communication with PC is done via PC_PORT. USB-to-UART converter should be used. Use of [ESP32 programmer](https://www.aliexpress.com/item/1005009724924926.html?spm=a2g0o.productlist.main.28.50a462a6PX678Z&algo_pvid=a313fb31-05b8-4d91-9cc8-3e550652c0a1&algo_exp_id=a313fb31-05b8-4d91-9cc8-3e550652c0a1-27&pdp_ext_f=%7B%22order%22%3A%2274%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21RSD%21330.34%21218.03%21%21%2121.03%2113.88%21%40211b80d117652208853908322edefe%2112000049958310724%21sea%21SRB%21932517771%21X%211%210%21n_tag%3A-29919%3Bd%3Af01a2faf%3Bm03_new_user%3A-29895&curPageLogUid=Jh888jSCBt18&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005009724924926%7C_p_origin_prod%3A) is recommended.
Device features a programming button FLASH_BTN (SW1), which should be activated during the upload.

BTN_PORT (X1) is used for connection with ButtonsPCB

<img width="585" height="185" alt="ButtonsPCB_Config" src="https://github.com/user-attachments/assets/63999b9b-8a74-411c-b920-6cbc90c46896" />


ButtonsPCB contains four pushbuttons used for defice configuration:
- **ESP_RST** (SW1)  - MCU reset
- **URL_CFG** (SW2)  - Defining URL for cloud service
- **WIFI_CFG** (SW3) - Wi-Fi connection configuration
- **WIFI_RST** (SW4) - Disconnecting from the current Wi-Fi network -> another click: deleting default Wi-Fi paarameters from the NVS.


Wi-Fi configuration should be done in communication with PC via PC_PORT.

