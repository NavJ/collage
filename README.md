Auto Collage
============

This is a small project written over winter break 2012-2013.

Here's an album of some collages made with this program: http://imgur.com/a/pD4HJ


Compilation
-----------

To compile, simply run one of the following in the top level directory:<br>
<code>make img</code><br>
<code>make img-u</code><br>
The first command will output a file <code>img</code>, and the second will output a file <code>img-u</code>. The actual functionality of both is the same, but the latter has assertions disabled to slightly improve runtime.


Execution
---------

Before you run the code, you must have a source image, shown as "source.jpg" in the example below (although .jpg, .gif, .bmp and .png are all supported). You must also have a folder containing the image files you wish to fit to the source image. More images in this folder means better results.

After you have compiled the code, run using the following (replace <code>img</code> with <code>img-u</code> if you prefer):<br>
<code>./img [source.jpg] [folder_of_images]</code><br>
First, the folder_of_images will be completed scanned and metadata will be generated for each of the images in the folder. This will be reused when possible. Once that has been completed, the collage will be generated as specified in image_util_test.c (soon I will move this to a flag/cli functionality instead of having to make manual changes to the code). Once generated, it will be saved to source_col.png.
