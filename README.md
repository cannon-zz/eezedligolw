# Overview

This is an an alternative to libmetaio based on the ezxml library.  The
advantage this offers over libmetaio is that it can handle the full LIGO
Light-Weight XML DTD.  For example, in addition to Table elements, which
libmetaio supports, Array, Param, and Time elements are supported, which
are enough to allow a PSD file to be read.

As with libmetaio, this library also does not solve the problem of
providing a comprehensive LIGO Light-Weight XML I/O and document
manipulation framework.  Nothing at all like the Python
[ligolw](https://git.ligo.org/kipp/python-ligo-lw) library's features are
available here.  This library provides a basic low-level framework upon
which to write code to extract data from LIGO Light-Weight XML documents.
No facility is provided to write data back to disk.

## Details

The entire LIGO Light-Weight DTD is supported, however specialized support
code is available only for Array, Param, Table and Time elements (and their
children).  See the ezligolw.h file for the library's public interface.
This should be enough for the majority of data analysis applications.

After being loaded into memory and parsed, Table element trees can be
translated into a generic, inefficient, internal representation, or an
external row building call-back can be provided to translate the data into
structures provided by the calling code.

A modest support library is provided to assist with using the parsing code
together with [lalsuite](https://git.ligo.org/lscsoft/lalsuite).
Facilities are provided to translate a limited selection of LSC tables from
LIGO Light-Weight XML files into linked lists of LAL table row structures,
and to extract LALDict dictionaries of REAL8FrequencySeries PSDs from XML
files.  See the examples/ directory for demonstrations.

ezxml reads the entire document into memory as a string, which typically
means enough memory is required, at least momentarily, to store two full
copies of the document:  the text copy ezxml loaded, and the decoded data
structures extracted from it.

The ezxml API uses plain char types everyhwere, and its unicode support is
hit and miss.  Assume you cannot use non-ASCII characters in your
documents.

## ezxml Status

ezxml is abandonware, but there are some places where forks of it live that
have received some updates.  I'll document the ones I know of here to make
it easier to find them again in the future.
- https://github.com/lxfontes/ezxml
- https://github.com/RT-Thread-packages/ezXML

Both of those claim to be version 0.8.5 but 0.8.6 existed before the
project was abandoned and a snapshot of 0.8.6 is what's in this tree.  It's
possible those repos, too, are forked from the latest snapshot and just
haven't updated the version number in their documentation.  I haven't
bothered doing a comparison of the histories.  The latter has some patches
that claim to improve unicode support, which might be good to include here.
