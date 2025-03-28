# opatch 

The ``patch``(1) implementation from OpenBSD, verbatim and portabilized per
[``baiacu.h``](./baiacu.h).  

## Why?

As always, competition is good, principally if it's offering more freedom,
and I didn't want to compile {stripped-,}lobase just for a single command.

## Where does it run?

Theoretically every Linux system and, of course, BSD.

## Chip in!

As usual, here are some ways you can chip in this project.

### How to update the source code?

Read [HOW2UPDATE.md](./HOW2UPDATE.md), it's too large to fit here.

### (Possible) bugs

This is basically the verbatim implementation from OpenBSD, but, as the
introduction to this file already mentioned, it is portabilized per a
single-header library called "baiacu", present at the ``baiacu.h`` file.  
Most of the functions implemented there were already tested and seem to work
really well but, since nothing is really bullet-proof until a whole lot of
testing, something might garble in the process.  
In this case, be aware that bugs such as memory faults can happen depending on
the platform and compiler. If it happens, please, [open an
issue](https://github.com/Projeto-Pindorama/opatch/issues/new) and mention what
happened, as precisely as possible.

## Licence

[BSD 2-Clause](./LICENSE.txt), but it varies from file to file.  
Copycenter licences in general.  
``baiacu.h`` is also under the BSD 2-Clause licence, so feel free (as in
freedom) to copy and paste it on any project that was originally intended to
run over BSD and that you wish to run on Linux (or any other UNIX-compatible
systems).  
If you implement anything new, I would be happy to know — and perhaps receive a
patch. :^)
