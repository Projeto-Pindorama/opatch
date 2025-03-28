# opatch 

The ``patch``(1) implementation from OpenBSD, verbatim and portabilized per
[``baiacu.h``](./baiacu.h).  

## Why?

As always, competition is good, principally if it's offering more freedom,
and I didn't want to compile {stripped-,}lobase just for a single command.

## Where does it run?

Theoretically every Linux system and, of course, BSD.

## (Possible) bugs

## How to update the source code

Enter the directory where opatch source tree is placed:

```console
S145% cd ~/projetos/opatch
```

Create a branch for the updated source code based on ``orig.180404``:

```console
S145% git branch
* main
  orig.180404
S145% git checkout orig.180404                                                                                                                     
Switched to branch 'orig.180404'
Your branch is up to date with 'origin/orig.180404'.
S145% git branch new
S145% git branch
  main
  new
* orig.180404
S145% git checkout new
Switched to branch 'new'
```

Get latest source from OpenBSD.org — see OpenBSD's ["Mirrors"
page](https://www.openbsd.org/ftp.html) and replace "``$version``"
with the value of the newest release:

```console
S145% cd /tmp
S145% curl -O#L https://cdn.openbsd.org/pub/OpenBSD/$version/src.tar.gz
S145% gunzip -cd src.tar.gz | tar -xvf - usr.bin/patch
x usr.bin/patch/CVS directory
x usr.bin/patch/CVS/Root 5 bytes, 1 tape blocks
x usr.bin/patch/CVS/Repository 18 bytes, 1 tape blocks
x usr.bin/patch/CVS/Entries 788 bytes, 2 tape blocks
x usr.bin/patch/CVS/Tag 13 bytes, 1 tape blocks
x usr.bin/patch/Makefile 157 bytes, 1 tape blocks
x usr.bin/patch/backupfile.c 6386 bytes, 13 tape blocks
x usr.bin/patch/backupfile.h 1009 bytes, 2 tape blocks
x usr.bin/patch/common.h 3094 bytes, 7 tape blocks
x usr.bin/patch/ed.c 6825 bytes, 14 tape blocks
x usr.bin/patch/ed.h 884 bytes, 2 tape blocks
x usr.bin/patch/inp.c 10980 bytes, 22 tape blocks
x usr.bin/patch/inp.h 1381 bytes, 3 tape blocks
x usr.bin/patch/mkpath.c 2386 bytes, 5 tape blocks
x usr.bin/patch/patch.1 19985 bytes, 40 tape blocks
x usr.bin/patch/patch.c 28192 bytes, 56 tape blocks
x usr.bin/patch/pch.c 38035 bytes, 75 tape blocks
x usr.bin/patch/pch.h 2041 bytes, 4 tape blocks
x usr.bin/patch/util.c 9042 bytes, 18 tape blocks
x usr.bin/patch/util.h 2111 bytes, 5 tape blocks
```

Return to opatch's source tree, in the branch created for receiving the new
source, copy the new files that were just extracted at /tmp and, then, add
and commit it:

```console
S145% cd -
/usr/home/luiz/projetos/opatch
S145% cp /tmp/usr.bin/patch/* .                                                                                                                    
cp: </tmp/usr.bin/patch/CVS> directory
S145% git add .
S145% git commit -m "feat: Import code from OpenBSD $version src.tar.gz"
```

```
S145% git merge main                                                                                                                               
Auto-merging Makefile
CONFLICT (content): Merge conflict in Makefile
Automatic merge failed; fix conflicts and then commit the result.
S145% git add Makefile
S145% git commit
[new 301dee3] Merge branch 'main' into new
S145% git checkout main
Switched to branch 'main'
Your branch is up to date with 'origin/main'.
S145% git merge new 
Updating 55574ab..301dee3
Fast-forward
 backupfile.c |  26 ++++++++++++++++++--------
 common.h     |   7 ++++---
 ed.c         |   6 +++---
 inp.c        |  13 +++++--------
 patch.1      |  31 ++++++++++++++-----------------
 patch.c      | 120 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++------------------------
 pathnames.h  |  11 -----------
 pch.c        | 101 ++++++++++++++++++++++++++++++++++++++++++++++++++++++-----------------------------------------------
 pch.h        |  19 +++++++++----------
 util.c       |  52 +++++++++++++++++++++++++++++++++-------------------
 util.h       |   3 ++-
 11 files changed, 238 insertions(+), 151 deletions(-)
 delete mode 100644 pathnames.h
S145% 
```

## Licence

[BSD 2-Clause](./LICENSE.txt), but it varies from file to file.  
Copycenter licences in general.
