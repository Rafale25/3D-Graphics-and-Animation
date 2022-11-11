# Final Render
![Final Render](Final_Render.png)

# Models
- Robot
- walls, ground, pillar
- wall light
- sledge hammer
- wooden crate
- wooden palette
- handsaw

<br>

# Robot

For the most part of the work, I did not have any idea of what I wanted the final scene to look like so I just started doing stuff. <br>
First model I made is a four legged robot inspired by the enemies in the indie game ScrapMechanic

#### The enemies in question:
<img src="images/scrapMechanic_bot.png" width="300"/>
<img src="images/scrapMechanic_bot2.png" width="400"/>

<br>

The robot is composed of 3 models: the body, the legs, and the arm.

I started with the **body**.
### ***process***:
Circle of 6 vertices, extruded vertically, filled the gaps.<br>
I used the auto-mirror addon to mirror the work done on one side.<br>
I used a UV sphere and a boolean modifier to make a sphere hole for the legs joints.<br>

<img src="images/body.png" width="50%"/>

As for the eye, i used inset on the front face, then subdivided the inner square and then translate the middle point with proportional editing to get the eye shape. (I change the eye later by a torus and a sphere because i realized it was simpler to make a model out of multiple shapes than the same connected mesh)<br>
<!-- Split this in 2 images and crop in interesting bits -->
<img src="images/body+eye.png" width="75%"/>

The arm is a single segment that I duplicated and put on top of each of other scaling it down a bit each time.<br>
<img src="images/arm_segment.png" width="49%"/>
<img src="images/arm.png" width="49%"/>


The leg is the most detailed part of the robot's mesh. But not very interesting to explain in details as it's just a lot of simple operation (A bunch of shapes put together, loop cuts, bevels ...)<br>
<img src="images/leg.png" width="100%"/>
<img src="images/leg2.png" width="24%"/>
<img src="images/leg1.png" width="24%"/>
<img src="images/leg_exploded1.png" width="24%"/>
<img src="images/leg_exploded2.png" width="24%"/>

## Armature
For the armature i used bones instead of simple object parenting because I wanted to use inverse kinematics to make the job of animating it for the Unreal Animation easier (turned out that IK is difficult)

<!-- constraints -->

<img src="images/skeleton1.png" width="49%"/>
<img src="images/skeleton2.png" width="49%"/>
<img src="images/pose.png" width="100%"/>


## Materials
For the robot's material I did not use any textures and I made a fully procedural "**Scratched Metal**" material


The entire material is encapsulated inside a group node as we can see here.
<img src="images/material_metal_node.png" width="100%"/>

What's inside the material (group node)
<img src="images/material_metal.png" width="100%"/>

The material is composed of 3 components
- The underlying metal
<img src="images/material_metal_underlying_metal.png" width="100%"/>
<img src="images/metal.png" width="50%"/>


- The paint
<img src="images/material_metal_paint.png" width="100%"/>
<img src="images/paint.png" width="30%"/>

- The edge wear
<img src="images/material_metal_edge_wear.png" width="100%"/>

    <p>no edge wear</p>
    <img src="images/no_edge_wear.png" width="20%"/>

    <br>
    <p>with edge wear</p>
    <img src="images/edge_wear.png" width="20%"/>

<br>

Everything put together
<img src="images/rusty_metal.png" width="100%"/>





<br>

---
<br>

- walls, ground, pillar
    - explain that I used free textures from internet and not procedural materials


- wall light
    - then I added a wall light with an emissive material as light source

> i wanted to have a few objects in background because of the project requirements and because the scene background was kinda empty
- sledge hammer
- wooden crate
- wooden palette
- handsaw


> explain each individually
- sledge hammer
    - simple to do, used free textures

- wooden crate
    - 2 textures

- wooden palette
    - 2 textures

- handsaw
    - use of the path tool and an image as blueprint then converted it to mesh (extruded, boolean for the center ...)

---

## Compositing
- Use of depth of field




# Resources

Almost all textures come from this website https://www.cgbookcase.com/

## modeling
https://youtu.be/WtNpGZGZbr0 <br>
https://youtu.be/O_Cyz9WBH9U <br>
https://youtu.be/g-4l3c42GaQ <br>

## procedural materials
https://youtu.be/hEH8TsGM78o <br>
https://youtu.be/_KXXyPd83s0 <br>
https://youtu.be/1iBEwV0BgCY <br>

## Armature / rigging
https://youtu.be/8Yb7_Ao6QHM <br>
https://youtu.be/yFxrqgUfbmc <br>
https://youtu.be/J1He21vDIfQ <br>

## Texturing / baking
https://youtu.be/HweuUl81qtw <br>
https://youtu.be/NQMtZi9j5d0 <br>
https://youtu.be/wG6ON8wZYLc <br>