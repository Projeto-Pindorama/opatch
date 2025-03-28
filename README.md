# opatch 

The ``patch``(1) implementation from OpenBSD, verbatim and portabilized per
[``baiacu.h``](./baiacu.h).  

## Why?

As always, competition is good, principally if it's offering more freedom,
and I didn't want to compile {stripped-,}lobase just for a single command.

## Where does it run?

Theoretically every Linux system and, of course, BSD.

## Compile and install

```console
S145% gmake && gmake install
```

Per default, it installs the "patch" program at ``/usr/ccs/bin`` and its
manual at ``/usr/ccs/share/man/man1``, with ``/usr/ccs`` being the prefix
for development tools at [Copacabana](https://copacabana.pindorama.net.br).
If you wish or need, you can change the instalation directories using the
variables ``$(ROOT)``, which is equivalent to ``$(DESTDIR)``, ``$(CCSBIN)``
and ``$(CCSMAN)``. Be careful: there is not a separate ``$(PREFIX)``
variable, you will need to set both the binary and manual page directories
manually.  
Remember to place the declarations _after_ the ``gmake`` call and before
``install``, like this:

```console
S145% gmake ROOT=... CCSMAN=... CCSBIN=... install
```

The same applies to ``CFLAGS`` and ``CPPFLAGS`` at the compilation stage:

```console
S145% gmake CFLAGS=...
```

## Chip in!

As usual, here are some ways you can chip in this project.

### How to update the source code?

Read [HOW2UPDATE.md](./HOW2UPDATE.md), it's too large to fit here.

### (Possible) bugs

This is basically the verbatim implementation from OpenBSD, but, as the
introduction to this file already mentioned, it is ([mostly](https://github.com/Projeto-Pindorama/opatch/commit/4558398f2ed7224577489cabeda400ccea667d5e#diff-9562c0bae0c928d1c4b9d6bd8ffd0a711f5771fb3d6c19e577e7f347fc4c2cf2))
portabilized per a single-header library called "baiacu", present at the
``baiacu.h`` file.  
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
