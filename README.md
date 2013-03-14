Auto Collage
============

This is a small project written over winter break 2012-2013.

Here's an album of some collages made with this program: http://imgur.com/a/pD4HJ


Compilation
-----------

To compile, simply run one of the following in the top level directory:<br>
<code>make img</code><br>
<code>make img-u</code><br>
The first command will output a file <code>img</code>, and the second will output a file <code>img-u</code>. The actual functionality of both is the same, but the latter has some tweaks to improve runtime.


Usage
-----

<code>./img [-k] [-v] -f folder -i image [-o output]</code><br>
OR <code>./img -p [-v] -f folder</code><br>

<code>-p</code> parses filder, then exits.<br>
<code>-k</code> uses kdtree algorithm to generate output image (faster runtime, but reuses images from the source directory).<br>
<code>-h</code> prints a help/usage message.<br>
<code>-v</code> toggles verbosity.<br>
