#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include "imdb.h"
#include "path.h"
using namespace std;

/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */

static string promptForActor(const string& prompt, const imdb& db)
{
  string response;
  while (true) {
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);
    if (response == "") return "";
    vector<film> credits;
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
	 << "Please try again." << endl;
  }
}

/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */

void generateShortestPath(string &source, string &target, imdb &db)
{
  list<path> paths; 
  set<string> act;
  set<film> films;

  path startPath(source); 
  paths.push_front(startPath); 

  while (!paths.empty() && paths.front().getLength() <= 5) {
    path p =paths.front();
    paths.pop_front(); 
    vector<film> movies;
    db.getCredits(p.getLastPlayer(), movies);

   
    for (int i = 0; i < movies.size(); i++) {
      if (films.find(movies[i]) == films.end()) {
	        films.insert(movies[i]); 
	        vector<string> cast;
	        db.getCast(movies[i],cast);
	
	
	  for ( int j = 0; j < cast.size(); j++) {
	   if (act.find(cast[j]) == act.end()) {
	    act.insert(cast[j]); 
	    path newP = p; 
	    newP.addConnection(movies[i], cast[j]); 
	    if (cast[j] == target) { 
	      cout << newP<< endl;
	      return; 
	    } else
	      paths.push_back(newP); 
	   }
	  }
      }
  }
}
  cout << endl << "No path between those two people could be found." << endl;
  }

int main(int argc, const char *argv[])
{
  imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    return 1;
  }
  
  while (true) {
    string source = promptForActor("Actor or actress", db);
    if (source == "") break;
    string target = promptForActor("Another actor or actress", db);
    if (target == "") break;
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } else {
      // replace the following line by a call to your generateShortestPath routine... 
      generateShortestPath(source, target, db);
    }
  }
  cout << "Thanks for playing!" << endl;
  return 0;
}