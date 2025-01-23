This is a demo of an alternative to libmetaio based on the ezxml library
discovered by somebody in the pulsar group, I think Greg Mendell.  In this
demo, a sim_burst table is parsed into a LAL MetadataTable linked list, and
then written back to disk using the old LAL routines, thereby demonstrating
a round-trip disk --> LAL --> disk via ezxml.

To run it, type "make", and you should get a program named "a.out".  Run
that, with no arguments.  While running a.out, note the time required to
parse and rewrite a 1/4 million row file.

ezxml reads the entire document into memory as a string, which poses a
potential scaling problem, but also makes it trivial to use it with gzipped
files.  You can either ask ezxml to parse a file by name, or load the file
first yourself and pass ezxml a pointer to the buffer.  Parsing a gzipped
xml file is then as simple as reading it with zlib, and then calling ezxml
on resulting buffer.

Also, ezxml is happy to work with memory-mapped files, which offers an
alternative work-around to scaling issues (of course, it's not easy to work
with a gzipped memory-mapped file).

If you've looked at the insides of libmetaio recently, you'll recognize
that ligolw.h and ligolw.c implement what is essentially the same API.
Starting with this code, it should take only an afternoon of tinkering to
have something that emulates enough of the libmetaio API to be used as a
drop-in replacement in LAL.  But if it was me, I wouldn't work on a drop-in
replacement, I would port the existing LAL reading codes to this thing or
some version of it.

Note that such a replacement would not move LAL's XML capabilities forward
in any substantial way.  The only immediately useful thing replacing
libmetaio with something like this would achieve is the fixing of the
attributes-are-not-orderless bug, which nevertheless is a serious bug and
one that will become more critical as people make more and more use of
glue.ligolw.  One other thing it does address is the problem of LALApps
programs being generally incapable of preserving the contents of an XML
file unless they know ahead of time what tables are contained inside it.
Because ezxml is a real XML parser, allowing the entire XML tree to be
parsed and then written back to disk, one could potentially have a LALApps
program preserve an arbitrary LIGO light-weight document's contents the way
a glue.ligolw program does.  And it could do this even if the document
contains things other than tables.  Although ezxml makes this possible, the
metaio API does not, so any emulation of the metaio API built out of ezxml
would prevent one from making use of this capability.  One would have to
abandon the metaio API.

