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
I also added rotation constraints on the leg tips and on the whole arm to make it easier to position later.

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

That's all for the robot !

<br>

> Next i started making the rest of the scene in where the robot was gonna be rendered.

<br>

# Environment
The environment is composed of 3 simple objects
- The floor
- 2 walls
- 3 pillars

<img src="images/environment_solid.png" width="100%"/>

For each of them I used free texture found on internet to make the materials.
Textures composed of **color**, **roughness**, **normals** and **displacement**.

<br>

To create a material using these textures, I create a new material.<br>
<img src="images/node_wrangler2.png" width="40%"/>

Then I use the node-wrangler builtin addon and select the BSDF and press **Ctrl + Shift + T**.
It open a file prompt where I select the 4 textures and it automatically link them by detecting the word in the file names.

<img src="images/node_wrangler.png" width="100%"/>

<br>

> explain the UV unwrapping


I do this for the 3 models and here is the result.
<img src="images/environment_render.png" width="100%"/>

<br>

# Wall light

Just some simple geometry with a dark black material and an emissive one for the light parts of the mesh.

<img src="images/wall_light_solid.png" width="49%"/>
<img src="images/wall_light_material.png" width="49%"/>
Placed in the scene.
<img src="images/scene_lights.png" width="100%"/>

<br>

# Sledge hammer

Simple model, nothing special, I used the node wrangler and some free textures for the metal head.

<img src="images/hammer_solid.png" width="20%"/>
<img src="images/hammer_render.png" width="14.2%"/>

<br>

# Wooden crate

Another simple model that uses 2 textures.
I had to use a colorRamp to lighten the wood and darken the other to make it look better.

<img src="images/crate_solid.png" width="40%"/>
<img src="images/crate_render.png" width="40%"/>

<br>

# Wooden palette

Again another simple model that uses 2 textures.
Had to move manually the uv map for the edge faces of the botton planks to place them at good places on the trunk texture.

<img src="images/palette_solid.png" width="49%"/>
<img src="images/palette_render.png" width="49%"/>

<br>

# Handsaw

I used a reference image for this model and the path tool to make the handle that i then converted to a mesh.

<img src="images/hand_saw_solid.png" width="49%"/>
<img src="images/hand_saw_render.png" width="49%"/>

<br><br>

# Compositing

For the final render I placed in some coherent way the simple models in the background of scene.
And i duplicated the robot and gave a different color to the new one. I use the armature to position them in like if they are fighting each other.


<img src="images/scene_solid.png" width="100%"/>

And a last touch for the render is some depth of field to give more attention on the robots fighting in front of the camera.

<br><br>

# Conclusion

I'm happy of what I've accomplished to do globally. But there's a lot of part that I would do differently if I had to do it again.<br>
For example, the idea of making an object of multiples meshes instead of a single connected one (like the robot's eye that I actually changed at the end). Because of that, I have lost a lot of time at the beginning and on the robot. The tail of the robot have this problem even thought i did not mention it.<br>
Also the robot's armature is difficult to work with and to select, i did not do a great job on parenting objects.
I couldn't make a walking animation for the unreal Animation because too difficult without Inverse Kinematics.

<br><br>

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
