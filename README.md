# AutoWallGen
Automatically generates walls for the layout of a map made with [Valve Hammer Editor](https://developer.valvesoftware.com/wiki/Valve_Hammer_Editor).

This program takes a [VMF file](https://developer.valvesoftware.com/wiki/VMF) as ***Input*** (where the VMF has a floor layout made of [Brushes](https://developer.valvesoftware.com/wiki/Brush))

The program will create a **new** VMF file as ***Output***, this file will contain the automatically Generated walls as well as the original layout. (the *new* walls will be in their own [Visgroup](https://developer.valvesoftware.com/wiki/Visgroup) and can be hidden easily)


## Directions

1. Create a VMF file with a layout, all floors must be the same thickness (16 units) and snapped to the grid
2. **Drag and Drop** the VMF file onto the **AutoWallGen.exe**
3. For best results, press **'n'** when prompted for custom parameters, then press **Enter**
4. A **new VMF file** will be created with the Automatically Generated Walls, it should be in the same folder as **AutoWallGen.exe**

### VMF Requirements

The ***Input*** layout must follow a few simple rules for the walls to be made correctly:
1. All floors must be Snapped to the Grid, and share the **same thickness** of **16 units** on the Vertical Z axis.
2. All floors must be a ***minimum*** of **64 by 64 units** in the **X and Y axis** for best results.
3. Floors must be aligned "flush" with each other (no gap) for intersection walls to be removed.
    - If there is a gap between 2 floors, then there will be a wall there
    - If you want a **Path** between the floors, the floors must be **Flush** with no gap
4. All side faces of each floor brush **MUST** be Vertical, aka the face is perpendicular to the XY Plane in Hammer. 
    - Ramps are allowed - but the sides must still be vertical, See image: [https://i.imgur.com/UFeyF9e.png](https://i.imgur.com/UFeyF9e.png)

A **sample VMF file** is provided which will give you an idea of how the layout should be setup. You can see how it looks in the images below.

## Example
![](https://i.imgur.com/QasEF5d.png)

### Process GIF
![](https://image.ibb.co/bEiamw/Auto_Wall_Gen_Gif_41.gif)



