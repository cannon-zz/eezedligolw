This is a demo of an alternative to libmetaio based on the ezxml library
discovered by somebody in the pulsar group, I think Greg Mendell.  In this
demo, a sim_burst table is parsed into a LAL MetadataTable linked list, and
then written back to disk using the old LAL routines, thereby demonstrating
a round-trip disk --> LAL --> disk via ezxml.

To run it, type "make", and you should get a program in examples/ named
sim_burst_example.  Run that, with no arguments.  It reads the .xml file in
that directory, extracts the sim_burst table, and writes it to a new file.

ezxml reads the entire document into memory as a string, which poses a
potential scaling problem, but also makes it trivial to use it with gzipped
files.  You can either ask ezxml to parse a file by name, or load the file
first yourself and pass ezxml a pointer to the buffer.  Parsing a gzipped
xml file is then as simple as reading it with zlib, and then calling ezxml
on resulting buffer.  (since writing this, LAL has acquired the ability to
work with gzip'ed files, also).

Also, ezxml is happy to work with memory-mapped files, which offers an
alternative work-around to scaling issues (of course, it's not easy to work
with a gzipped memory-mapped file).

If you've looked at the insides of libmetaio recently, you'll recognize
that ligolw.h and ligolw.c implement what is essentially the same API.
Starting with this code, it should take only an afternoon of tinkering to
have something that emulates enough of the libmetaio API to be used as a
drop-in replacement within LAL.  But if it was me, I wouldn't work on a
drop-in replacement, I would port the existing LAL reading codes to this
thing or some version of it.  Doing that opens the doors to Array I/O, and
LAL would finally be able to read a PSD ... sigh (I mean, who would've
thought you'd need to do that, ever).
