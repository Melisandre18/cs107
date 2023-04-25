using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "imdb.h"



const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

struct info
{
	const void *value;
	const void *file;
};

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}


int compAct(const void * act1, const void * act2)
{
	info* data = (info*)act1;
  char * ptr = (char *)data->value;
	char * name = (char *)((char *)data->file + *(int *)act2);

	return strcmp(ptr, name);
	
}

int compMov(const void * mov1, const void * mov2)
{
	info *data = (info*)mov1;
	char * name = (char *)((char *)data->file + *(int *)mov2);
	int year = *(char *)(name + strlen(name) + 1);

	film mov;
	mov.title = name;
	mov.year = year + 1900;
  film val= *(film *)data->value;
	if(mov < val)  return 1;
	else if(val< mov)  return -1;

	return 0;
}


// you should be implementing these two methods right here... 

bool imdb::getCredits(const string& player, vector<film>& films) const 
{	
	int count = *(int *)actorFile;
	char* ptr = (char *)actorFile;
	info data;
	data.value = player.c_str();
	data.file = actorFile;

	int * ptrs = (int *)bsearch(&data,  ptr + sizeof(int), count, 
											sizeof(int), compAct);
	if(ptrs == NULL) return false; 

	char * pos = ptr+ *ptrs;
	int len = strlen(pos) % 2 == 1 ? strlen(pos) + 1 : strlen(pos) + 2;
	short *num = (short *)(pos+ len);
	len += sizeof(short);
	int *frst = len%4==0 ? (int *)(num + 1) : (int *)((char *)num + sizeof(short) + 2);
	
	for(int i = 0; i < *num; i++)
	{
		char * curMov = (char *)movieFile + *(frst + i);
		int curyr= *(char *)(curMov+ strlen(curMov) + 1);
		film flm;
		flm.title = curMov;
		flm.year = curyr + 1900;
		films.push_back(flm);
	}

	return true;
}

bool imdb::getCast(const film& movie, vector<string>& players) const
{
	int count = *(int *)movieFile;
  char* ptr = (char *)movieFile;

	info data;
	data.value = &movie;
	data.file = movieFile;
	int *ptrs = (int *)bsearch(&data, ptr + sizeof(int), count,
											sizeof(int), compMov);
	if(ptrs == NULL) return false;

	char *pos = ptr+ *ptrs;
	int len = (strlen(pos) + 1) % 2 == 1 ? strlen(pos) + 2 : strlen(pos) + 3;
	short *num = (short *)(pos + len);
	len += sizeof(short);
	int *frst = len%4==0 ? (int *)(num + 1) : (int *)((char *)num + sizeof(short) + 2);

	for(int i = 0; i < *num; i++)
	{
		int curPos = *(frst + i); 
		char * curPly = (char *)actorFile + curPos;
		players.push_back(curPly);
	}

	return true;
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
