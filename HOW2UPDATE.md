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
page](https://www.openbsd.org/ftp.html) and replace "``X.Y``"
with the value of the newest release, which is, currently, 7.6:

```console
S145% cd /tmp
S145% curl -O#L https://cdn.openbsd.org/pub/OpenBSD/X.Y/src.tar.gz
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
S145% git commit -m "feat: Import code from OpenBSD X.Y src.tar.gz"
```

Now, merge opatch's code to the new one. Some conflicts may appear in the
process:

```console
S145% git merge main                                                                                                                               
Auto-merging Makefile
CONFLICT (content): Merge conflict in Makefile
Automatic merge failed; fix conflicts and then commit the result.
S145% git add Makefile
S145% git commit
[new 301dee3] Merge branch 'main' into new
```

After that, try to compile the patch program and run some tests:

```console
S145% gmake
```

In case of something garbling, report it as an
[issue](https://github.com/Projeto-Pindorama/opatch/issues/new) — or, if
you're willing to chip in the maintenance, report it __and__ try to find a
solution.  
In this example, using OpenBSD 7.6 source code, we can see that they abandoned
the ``D_NAMLEN()`` macro and started referencing the ``d_namlen`` field
directly.

```console
cc -include baiacu.h -c backupfile.c
backupfile.c: In function ‘max_backup_version’:
backupfile.c:109:25: error: ‘struct dirent’ has no member named ‘d_namlen’; did you mean ‘d_name’?
  109 |                 if (dp->d_namlen <= file_name_length)
      |                         ^~~~~~~~
      |                         d_name
gmake: *** [Makefile:19: backupfile.o] Error 1
```

This can (and should) be altered just per replacing ``dp->d_namlen`` back with
the macro. ``sed``(1) must do the job:

```console
S145% cp backupfile.c{,.orig}
S145% sed '/d_namlen/s/\(.*\)\(dp\)->d_namlen\(.*\)/\1D_NAMLEN(\2)\3/' backupfile.c.orig > backupfile.c 
S145% git diff .
diff --git a/backupfile.c b/backupfile.c
index 3197d73..196a2af 100644
--- a/backupfile.c
+++ b/backupfile.c
@@ -106,7 +106,7 @@ max_backup_version(const char *file, const char *dir)
        file_name_length = strlen(file);
 
        while ((dp = readdir(dirp)) != NULL) {
-               if (dp->d_namlen <= file_name_length)
+               if (D_NAMLEN(dp) <= file_name_length)
                        continue;
 
                this_version = version_number(file, dp->d_name, file_name_length);
S145%
```

Repeat the process in search of anything that needs to be portabilized in the
code itself.  
In case of an error that involves an undefined function and/or an undefined
symbol, such as this:

```console
S145% gmake
cc -include baiacu.h -c backupfile.c
cc -include baiacu.h -c ed.c
cc -include baiacu.h -c inp.c
cc -include baiacu.h -c mkpath.c
cc -include baiacu.h -c patch.c
patch.c: In function ‘main’:
patch.c:210:13: error: implicit declaration of function ‘unveil’ [-Wimplicit-function-declaration]
  210 |         if (unveil(tmpdir, "rwc") == -1) {
      |             ^~~~~~
patch.c: In function ‘get_some_switches’:
patch.c:552:9: error: ‘optreset’ undeclared (first use in this function)
  552 |         optreset = optind = 1;
      |         ^~~~~~~~
patch.c:552:9: note: each undeclared identifier is reported only once for each function it appears in
gmake: *** [Makefile:19: patch.o] Error 1
```

The modifications must be made to the compatibility header, ``baiacu.h``. Good
references at times like these are the [Gnulib
manual](https://www.gnu.org/software/gnulib/manual/gnulib.html) and, of course,
OpenBSD manual pages for [library
functions](https://man.openbsd.org/?query=&sec=3) and
[syscalls](https://man.openbsd.org/?query=&sec=2).  
After all of this is done, commit every change:

```console
S145% git add backupfile.c
S145% git commit -m 'fix: Use D_NAMLEN() again instead of referencing the struct field directly.' 
[new 6ca1726] fix: Use D_NAMLEN() again instead of referencing the struct field directly.
 1 file changed, 1 insertion(+), 1 deletion(-)
S145% git add baiacu.h 
S145% git commit -m 'chore: Added false code for unveil() at baiacu.h.'
[new 4c1a7fa] chore: Added false code for unveil() at baiacu.h.
 1 file changed, 321 insertions(+)
 create mode 100644 baiacu.h
S145% git add patch.c 
S145% git commit -m 'fix: optind = !optreset at patch.c'
[new a02e44d] fix: optind = !optreset at patch.c
 1 file changed, 8 insertions(+)
```

Then return to the main branch and do the merge.

```console
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

***Alternatively***, you can push it to GitHub, where this project is being
hosted and maintained, and try to do an automatized merge from there:

```console
S145% git push -uv origin new  
Pushing to github.com:Projeto-Pindorama/opatch.git
Enter passphrase for key '/usr/home/luiz/id_xxxxxx': 
Enumerating objects: 25, done.
Counting objects: 100% (25/25), done.
Delta compression using up to 8 threads
Compressing objects: 100% (13/13), done.
Writing objects: 100% (13/13), 3.58 KiB | 1.79 MiB/s, done.
Total 13 (delta 11), reused 0 (delta 0), pack-reused 0 (from 0)
remote: Resolving deltas: 100% (11/11), completed with 11 local objects.
remote: 
remote: Create a pull request for 'new' on GitHub by visiting:
remote:      https://github.com/Projeto-Pindorama/opatch/pull/new/new
remote: 
To github.com:Projeto-Pindorama/opatch.git
 * [new branch]      new -> new
branch 'new' set up to track 'origin/new'.
updating local tracking ref 'refs/remotes/origin/new'
```

![Image](https://github.com/user-attachments/assets/e474125a-a101-4a8b-ba12-ce4297ad9bf6)
![image](https://github.com/user-attachments/assets/65235215-b7b3-41c3-b766-fe1070fe2963)

After that all that, delete the "new" branch:

```console
S145% git branch -d new
Deleted branch new (was 301dee3).
```

And, in case of merging locally, push the local main branch to remote origin:

```console
S145% git push -uv origin main
Pushing to github.com:Projeto-Pindorama/opatch.git
Enter passphrase for key '/usr/home/luiz/id_xxxxxx': 
Enumerating objects: 31, done.
Counting objects: 100% (27/27), done.
Delta compression using up to 8 threads
Compressing objects: 100% (17/17), done.
Writing objects: 100% (17/17), 2.29 KiB | 1.15 MiB/s, done.
Total 17 (delta 11), reused 0 (delta 0), pack-reused 0 (from 0)
remote: Resolving deltas: 100% (11/11), completed with 5 local objects.
To github.com:Projeto-Pindorama/opatch.git
   d0088c2..cd1e74c  main -> main
branch 'main' set up to track 'origin/main'.
updating local tracking ref 'refs/remotes/origin/main'
```

***Alternatively***, in case of having merged it remotely via GitHub,
you __will need__ to update the main branch. Otherwise, you may have big
problems when trying to mess with the source code tree in the future:

```console
S145% git pull
Enter passphrase for key '/usr/home/luiz/id_xxxxxx': 
remote: Enumerating objects: 68, done.
remote: Counting objects: 100% (46/46), done.
remote: Compressing objects: 100% (10/10), done.
remote: Total 30 (delta 22), reused 28 (delta 20), pack-reused 0 (from 0)
Unpacking objects: 100% (30/30), 5.77 KiB | 18.00 KiB/s, done.
From github.com:Projeto-Pindorama/opatch
   d0088c2..cd1e74c  main       -> origin/main
Updating d0088c2..cd1e74c
Fast-forward
 backupfile.c |  24 +++++++++++++++++-------
 baiacu.h     |   7 ++++---
 common.h     |   7 ++++---
 ed.c         |   6 +++---
 inp.c        |  13 +++++--------
 patch.1      |  31 ++++++++++++++-----------------
 patch.c      | 126 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++-----------------------
 pathnames.h  |  11 -----------
 pch.c        | 101 ++++++++++++++++++++++++++++++++++++++++++++++++++++++-----------------------------------------------
 pch.h        |  19 +++++++++----------
 util.c       |  52 +++++++++++++++++++++++++++++++++-------------------
 util.h       |   3 ++-
 12 files changed, 248 insertions(+), 152 deletions(-)
 delete mode 100644 pathnames.h
```

Well, I think that's it.
