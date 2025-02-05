#include <stdlib.h>
#include <zlib.h>
#include <libezligolw/ezligolw.h>


/*
 * equivalent to ezxml_parse_file() but transparently decompresses
 * gzip-compressed files if that format is detected
 */


ezxml_t ligolw_parse_file(const char *path)
{
	gzFile f;
	char *buf = NULL;;
	size_t alloc_size = 65536;
	size_t pos = 0;
	size_t l;

	f = gzopen(path, "rb");
	if(!f)
		return NULL;

	/* set size of input buffer */
	gzbuffer(f, 131072);

	do {
		char *newbuf = realloc(buf, alloc_size *= 2);
		if(!newbuf) {
			gzclose(f);
			free(buf);
			return NULL;
		}
		buf = newbuf;

		l = gzread(f, buf + pos, alloc_size - pos);
		pos += l;
	} while(pos == alloc_size);

	gzclose(f);

	return ezxml_parse_str(buf, pos);
}
