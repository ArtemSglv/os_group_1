/*
	gcc -Wall task5.c `pkg-config fuse3 --cflags --libs` -o task5
*/

#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

static char *bar_path = "/bar";
static char *baz_path = "/bar/baz";
static char *bin_path = "/bar/baz/bin";
static char *foo_path = "/bar/baz/foo";
static char *cp_path = "/bar/baz/foo/cp";
char testText[106];
static char *system_cp_path = "/bin/cp";

static struct options
{
	const char *path;
	char *contents;
} readme, example, test;

size_t filesize(const char *filename) {
	struct stat st;
	size_t retval = 0;
	if (stat(filename, &st))
		printf("cannot stat %s\n", filename);
	else 
		retval = st.st_size;
	return retval;
}

static int file_getattr(const char *path, struct stat *stbuf,
		struct fuse_file_info *fi)
{
	(void) fi;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0)
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}
	else if (strcmp(path, bar_path) == 0)
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}
	else if (strcmp(path, baz_path) == 0)
	{
		stbuf->st_mode = S_IFDIR | 0744;
		stbuf->st_nlink = 2;
	}
	else if (strcmp(path, bin_path) == 0)
	{
		stbuf->st_mode = S_IFDIR | 0177;
		stbuf->st_nlink = 2;
	}
	else if (strcmp(path, readme.path) == 0)
	{
		stbuf->st_mode = S_IFREG | 0544;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(readme.contents);
	}
	else if (strcmp(path, example.path) == 0)
	{
		stbuf->st_mode = S_IFREG | 0555;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(example.contents);
	}
	else if (strcmp(path, foo_path) == 0)
	{
		stbuf->st_mode = S_IFDIR | 0711;
		stbuf->st_nlink = 2;
	}
	else if (strcmp(path, cp_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = filesize(system_cp_path);
	}
	else if (strcmp(path, test.path) == 0) {
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(test.contents);
	}	
	else
		return -ENOENT;

	return 0;
}

static int file_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi,
		enum fuse_readdir_flags flags)
{
	(void)offset;
	(void)fi;
	(void)flags;

	int ch = (int)'/';

	if (strcmp(path, "/") == 0)
	{
		filler(buf, ".", NULL, 0, 0);
		filler(buf, "..", NULL, 0, 0);
		filler(buf, bar_path + 1, NULL, 0, 0);		
	}
	else if (strcmp(path, bar_path) == 0)
	{
		filler(buf, ".", NULL, 0, 0);
		filler(buf, "..", NULL, 0, 0);
		filler(buf, baz_path +(int)(strrchr(baz_path, ch)-baz_path)+1, NULL, 0, 0);
	}
	else if (strcmp(path, baz_path) == 0)
	{
		filler(buf, ".", NULL, 0, 0);
		filler(buf, "..", NULL, 0, 0);
		filler(buf, bin_path +(int)(strrchr(bin_path, ch)-bin_path)+1, NULL, 0, 0);
		filler(buf, readme.path +(int)(strrchr(readme.path, ch)-readme.path)+1, NULL, 0, 0);
		filler(buf, example.path +(int)(strrchr(example.path, ch)-example.path)+1, NULL, 0, 0);
		filler(buf, foo_path +(int)(strrchr(foo_path, ch)-foo_path)+1, NULL, 0, 0);
	}
	else if (strcmp(path, foo_path) == 0) {
		filler(buf, ".", NULL, 0, 0);
		filler(buf, "..", NULL, 0, 0);
		filler(buf, cp_path +(int)(strrchr(cp_path, ch)-cp_path)+1, NULL, 0, 0);
		filler(buf, test.path +(int)(strrchr(test.path, ch)-test.path)+1, NULL, 0, 0);
		
	}	
	else
		return -ENOENT;

	return 0;
}

static int file_read(const char *path, char *buf, size_t size, off_t offset,
			struct fuse_file_info *fi)
{
	size_t length;
	(void)fi;
	struct options to_read;

	if (strcmp(path, cp_path) == 0)
	{
		FILE *fcp = fopen(system_cp_path, "rb");
		unsigned char c;
		while (fread(&c, 1, 1, fcp))
			putchar(c);
		fclose(fcp);
		return size;
	}
	else if (strcmp(path, example.path) == 0) {
		to_read = example;
	}
	else if (strcmp(path, readme.path) == 0) {
		to_read = readme;
	}
	else if (strcmp(path, test.path) == 0) {
		to_read = test;
	}
	else 
		return -ENOENT;

	length = strlen(to_read.contents);
	if (offset < length) {
		if (offset + size > length)
			size = length - offset;
		memcpy(buf, to_read.contents + offset, size);
	}
	else
		size = 0;
	
	return size;
}

char *file_change_letters(const char *initial, int pos, int length, const char *new)
{
	int initial_length =  strlen(initial);
	int sub_length = strlen(new);
	
	if (length < 1 || pos < 0 || pos> initial_length || (pos+length) > initial_length)
		return NULL;

	char *result = NULL;
	
	int result_length = initial_length - length + sub_length;

	result = calloc(result_length, sizeof(char));
	memcpy(result, initial, pos);
	memcpy(result + pos, new, sub_length);
	memcpy(result + pos + sub_length, initial + pos + length, initial_length - (pos+length));
	return result;
}


static int file_rename(const char *initial, const char *new)
{
	int pos, length;
	int ch = (int)'/';

	if (strcmp(initial, bar_path) == 0)
	{
		char *initial_bar_name = bar_path + 1;
		length = strlen(initial_bar_name);

		bar_path = strdup(new);
		char *new_bar_name = bar_path + 1;

		pos = (int)(strstr(baz_path, initial_bar_name) - baz_path);
		baz_path = file_change_letters(baz_path, pos, length, new_bar_name);		
		
		pos = (int)(strstr(bin_path, initial_bar_name) - bin_path);
		bin_path = file_change_letters(bin_path, pos, length, new_bar_name);		
		
		pos = (int)(strstr(foo_path, initial_bar_name) - foo_path);
		foo_path = file_change_letters(foo_path, pos, length, new_bar_name);		

		pos = (int)(strstr(readme.path, initial_bar_name) - readme.path);
		readme.path = strdup(file_change_letters(readme.path, pos, length, new_bar_name));		
		pos = (int)(strstr(example.path, initial_bar_name) - example.path);
		example.path = strdup(file_change_letters(example.path, pos, length, new_bar_name));		
		pos = (int)(strstr(cp_path, initial_bar_name) - cp_path);
		cp_path = file_change_letters(cp_path, pos, length, new_bar_name);		

		pos = (int)(strstr(test.path, initial_bar_name) - test.path);
		test.path = strdup(file_change_letters(test.path, pos, length, new_bar_name));
	}
	else if (strcmp(initial, baz_path) == 0)
	{
		char *initial_baz_name = baz_path + (int)(strrchr(baz_path, ch) - baz_path + 1);
		length = strlen(initial_baz_name);

		baz_path = strdup(new);
		char *new_baz_name = baz_path + (int)(strrchr(baz_path, ch) - baz_path + 1);

		pos = (int)(strstr(bin_path, initial_baz_name) - bin_path);
		bin_path = file_change_letters(bin_path, pos, length, new_baz_name);		
		
		pos = (int)(strstr(foo_path, initial_baz_name) - foo_path);
		foo_path = file_change_letters(foo_path, pos, length, new_baz_name);		

		pos = (int)(strstr(readme.path, initial_baz_name) - readme.path);
		readme.path = strdup(file_change_letters(readme.path, pos, length, new_baz_name));		
		pos = (int)(strstr(example.path, initial_baz_name) - example.path);
		example.path = strdup(file_change_letters(example.path, pos, length, new_baz_name));		
		pos = (int)(strstr(cp_path, initial_baz_name) - cp_path);
		cp_path = file_change_letters(cp_path, pos, length, new_baz_name);		

		pos = (int)(strstr(test.path, initial_baz_name) - test.path);
		test.path = strdup(file_change_letters(test.path, pos, length, new_baz_name));
	}
	else if (strcmp(initial, bin_path) == 0)
	{
		bin_path = strdup(new);
	}
	else if (strcmp(initial, readme.path) == 0)
	{
		readme.path = strdup(new);
	}
	else if (strcmp(initial, example.path) == 0)
	{
		example.path = strdup(new);
	}
	else if (strcmp(initial, foo_path) == 0)
	{
		char *initial_foo_name = foo_path + (int)(strrchr(foo_path, ch) - foo_path + 1);
		length = strlen(initial_foo_name);

		foo_path = strdup(new);
		char *new_foo_name = foo_path + (int)(strrchr(foo_path, ch) - foo_path + 1);

		pos = (int)(strstr(cp_path, initial_foo_name) - cp_path);
		cp_path = file_change_letters(cp_path, pos, length, new_foo_name);		

		pos = (int)(strstr(test.path, initial_foo_name) - test.path);
		test.path = strdup(file_change_letters(test.path, pos, length, new_foo_name));		
	}
	else if (strcmp(initial, cp_path) == 0)
	{
		cp_path = strdup(new);
	}
	else if (strcmp(initial, test.path) == 0)
	{
		test.path = strdup(new);
	}
	else
		return -ENOENT;

	return 0;
}

static struct fuse_operations operations = 
{
	.read = file_read,
	.readdir = file_readdir,
	.getattr = file_getattr,
	.rename = file_rename,
};

int main(int argc, char *argv[])
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	example.path = strdup("/bar/baz/example");
	example.contents = strdup("Hello, world\n");
	readme.path = strdup("/bar/baz/readme.txt");
	readme.contents = strdup(" Студент Сергей Рузин #16150016\n");
	test.path = strdup("/bar/baz/foo/test.txt");
	for(int i=0;i<16;i++){
     testText[i*2] = 'a';
     testText[i*2+1] = '\n';
}
      testText[105] = '\0';
       test.contents = strdup(testText);
    
	return fuse_main(args.argc, args.argv, &operations, NULL);
}
