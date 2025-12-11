This document provides parameters for DIY PCB build. Manufacturing files are given.

## PCB clad
One sided PCB clad  
Copper thickness: 35 um  
Isolation thickness:  0.8 mm
___

## Exposure
Method: Photolithography on dry UV-sensitive film  
[UV film](https://www.aliexpress.com/item/1005001847878963.html?spm=a2g0o.productlist.main.5.160fFuobFuobZT&algo_pvid=9a28c199-a453-4ce3-bd24-f5ab4ee7d090&algo_exp_id=9a28c199-a453-4ce3-bd24-f5ab4ee7d090-4&pdp_ext_f=%7B%22order%22%3A%22448%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21RSD%21606.85%21605.73%21%21%215.41%215.40%21%40211b441e17639896731701124eebe9%2112000020690882508%21sea%21SRB%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3Af01a2faf%3Bm03_new_user%3A-29895%3BpisId%3A5000000193041959&curPageLogUid=KD6F865099se&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005001847878963%7C_p_origin_prod%3A#nav-specification) warm laminated on clean copper layer  
Exposure method: Direct imaging using monochrome Stereolithography LCD  
Device: Anycubic Photon Mono M5s Pro  
Display type: Monochrome 10.1" LCD, UV light @ 405 nm  
Image used: Positive STL model of PCB traces  
Exposure mode: Single-frame light exposure  
Exposure time: 40 seconds  
___

## Developing
Solution: 1 mass% Na2Co3  
Temperature: 25 °C  
Agitation: Light scrubbing with soft brush  
___

## Etching
Solution: 40 mass% FeCl3 | pH = 1  
Temperature 55 °C  
Agitation: Horizontal cyclical motion with 80 strokes/min  
___

## Soldermask
Method: Dry-film UV curing soldermask  
Soldermask type: [Dynamask 5000](https://www.ebay.com/itm/285613425122) | Warm laminated on clean copper layer  
Photomask: Positive image of soldermaks-free positions  
Photomask material: OHP transparent foil | thickness = 100 um  
Printer: HP MFP-178 | HQ BW mode  
UV lamp: 4x 9 W U-Shaped UV bulbs @ 365 nm  
Exposure time: 60 s  
Developing: Same as PCB UV film | 1 mass% Na2Co3 @ 25 °C and light scrubbing with soft brush  
Final exposure time: 1h  
___
