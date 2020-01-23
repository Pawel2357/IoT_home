## Battery setup
The battery is made of Nissan leaf modules. Each module is made of 4 cells.  
Charging characteristic https://qnovo.com/inside-the-battery-of-a-nissan-leaf/

## Charger
Epever AN 20A
TO-DO: save configuration of charging profile.

## Battery <-> home connection
Battery is connected to array of relays. Each relay is controlled by arduino with NodeMcu Esp. 
The array of relays is connected to automatic transfer switches. Each switch connects two sources of energy. When battery is charged
the home is powered using renovable energy. If battery is discharged the power source is set to grid. The device automatically switches to grid in case of 
battery not working.
