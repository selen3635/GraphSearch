/*
 * Name: Ziying Hu & Xiaolong Zhou
 * Login: cs100vat & cs100vcc
 * Date: Aug 29,, 2016
 * File: actorconnections.cpp
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <utility>
#include <chrono>
#include <climits>
#include <map>

using namespace std;

/* the class for timer */
class Timer {
  
  private:
    chrono::time_point<chrono::high_resolution_clock> start;

  public:

    /** Function called when starting the timer */
    void begin_timer() {

      start = std::chrono::high_resolution_clock::now();

    }

    /** Function called when ending the timer */
    double end_timer() {

      std::chrono::time_point<std::chrono::high_resolution_clock> end;
      end = std::chrono::high_resolution_clock::now();

      return std::chrono::duration_cast<std::chrono::nanoseconds>
        (end-start).count() * 1/10.0*1/10.0*1/10.0*1/10.0*1/10.0*1/10.0;
    }
};




struct Edge;
struct actors;


/******************************Struct for BFS*******************************/
/* the struct for actor nodes */
struct actors {
  // The name of actor
  string actor_name;
  // The adjacency list
  vector<Edge*> adj;
  // The earliest year of the actors acting movies
  int first_year = INT_MAX;
  bool visited = false;
};

/* the struct for edge */
struct Edge {
  int year;
  actors* left;
  actors* right;
};



/*******************************Struct for Union Find************************/
/* the struct for vertex */
struct u_actor {
  // The name of actor
  string name;
  // The earliest year of the actors acting movies
  int first_year = INT_MAX;
  // pointer to parent
  u_actor* parent = nullptr;
  // the number in the subtree
  int weight = 1;
};

/* the struct for movies */
struct movies {
  int year;
  vector<string> cast;
};

/* the struct for edges */
struct link {
  int year;
  u_actor* up;
  u_actor* down;
};



/* Traverse the graph using BFS */
void BFS ( struct actors* actor1, string toFind, ofstream& out ) {
  int start_year = actor1->first_year;
  while ( start_year < 2016 ) {
    /* the structure to store the vertex */
    queue<actors*> toExplore;
    vector<actors*> changed;
    actors* start = actor1;
    start->visited = true;
    changed.push_back(start);
    toExplore.push(start);

    /* if the queue is not empty, still doing BST */
    while ( !toExplore.empty() ) {
      /* pop the first node */
      actors* next = toExplore.front();
      toExplore.pop();

      /* traverse the adjacency list */
      for ( unsigned int i = 0; i < next->adj.size(); i++ ) {
        /* only traverse those year less than current year */
        if ( next->adj[i]->year <= start_year ) {
          /* update the neighbor according to edge */
          actors* neighbor;
          if ( next->adj[i]->left == next )
            neighbor = next->adj[i]->right;
          else
            neighbor = next->adj[i]->left;
          /* check whether it is visted */
          if ( !neighbor->visited ) {
            neighbor->visited = true;
            changed.push_back(neighbor);
            toExplore.push(neighbor);

            /* check whether have found the actor */
            if ( toFind.compare(neighbor->actor_name) == 0 ) {
              out << start_year << endl;

              /* changed the actors back */
              for ( unsigned int j = 0; j < changed.size(); j++ )
                changed[j]->visited = false;

              return;
            }// end of check
          }
        }
      }// end of traverse
    }
    start_year++;
    /* change all the nodes back */
    for ( unsigned int i = 0; i < changed.size(); i++ ) 
      changed[i]->visited = false;
  }// end of start year

  /* if still not found the connection until 2016 */
  out << "9999" << endl;
}


/* union the two actors */
void merge( u_actor* A, u_actor* B ) {
  vector<u_actor*> compression;
  /* if both A and B are not sentinels */
  u_actor* currA = A;
  while ( currA->parent != nullptr ) {
    compression.push_back(currA);
    currA = currA->parent;
  }
  u_actor* currB = B;
  while ( currB->parent != nullptr ) {
    compression.push_back(currB);
    currB = currB->parent;
  }

  /* if A and B are in the same tree */
  if ( currA == currB )
    return;

  /* if currA has less nodes */
  if ( currA->weight < currB->weight ) {
    currA->parent = currB;
    currB->weight = currA->weight + currB->weight;
    for ( unsigned int i = 0; i < compression.size(); i++ ) 
      compression[i]->parent = currB;
  }
  /* if currB has less nodes */
  else {
    currB->parent = currA;
    currA->weight = currA->weight + currB->weight;
    for ( unsigned int i = 0; i < compression.size(); i++ ) 
      compression[i]->parent = currA;
  }
}


/* find the sentinel node of that node */
u_actor* find ( u_actor* A ) {
  vector<u_actor*> compression;
  /* if A is not sentinels */
  u_actor* currA = A;
  while ( currA->parent != nullptr ) {
    compression.push_back(currA);
    currA = currA->parent;
  }
  /* changed those nodes parent to the sentinel */
  for ( unsigned int i = 0; i < compression.size(); i++ ) 
    compression[i]->parent = currA;
  return currA;
}

/* the main driver of the program */
int main ( int argc, char * argv[] ) {

  /* check for argument */
  if ( argc != 4 && argc != 5 ) {
    cout << argv[0] << " called with incorrect arguments.\n"
      << "Usage: " << argv[0] << " movie_cast_tsv_file pairs_tsv_file output_"
      << "cast_tsv_file ufind/bfs\n"
      << "If the last arguments if not specified the union find implementation"
      << "is used as default" << endl;
    return -1;
  }

  bool using_bfs = false;
  /* runnig bfs if it is specified */
  if ( argc == 5 ) {
    string argv4 = argv[4];
    if ( argv4 == "bfs" )
      using_bfs = true;
  }


  /* if using union find */
  if ( !using_bfs ) {
    ifstream in;
    in.open(argv[1], ios::binary);

    //cout << "Reading " << argv[1] << " ..." << endl;

    /* if it is an invalid file */
    if ( !in.is_open() ) {
      cout << "Failed to read " << argv[1] << "!\n";
      cout << argv[0] << " called with incorrect arguments.\n"
      << "Usage: " << argv[0] << " movie_cast_tsv_file pairs_tsv_file output_"
      << "cast_tsv_file ufind/bfs\n"
      << "If the last arguments if not specified the union find implementation"
      << "is used as default" << endl;
      return -1;
    }

    bool have_header = false;
    unordered_map<string, vector<string>>moviesTable;
    unordered_map<string, u_actor*> actorsTable;

    /* keep reading lines until the end of file is reached */
    while ( in ) {

      string myString;

      /* get the next line */
      if ( !getline( in, myString ) )
        break;

      /* skip the header */
      if ( !have_header ) {
        have_header = true;
        continue;
      }

      istringstream ss(myString);
      vector<string> record;

      /* get the next string before hitting a tab character and put in next */
      while ( ss ) {
        string next;
        if ( !getline( ss, next, '\t' ) )
          break;
        record.push_back( next );
      }

      /* we should have exactly 3 columns */
      if ( record.size() != 3 )
        continue;

      /* if the actors not exist in the table */
      if ( actorsTable.count(record[0]) == 0 ) {
        u_actor* myActor = new u_actor();
        myActor->name = record[0];
        actorsTable[record[0]] = myActor;
      }

      /* update the earliest year */
      int year = stoi(record[2]);
      if ( actorsTable[record[0]]->first_year > year )
        actorsTable[record[0]]->first_year = year;

      /* create a movie_header string */
      string movie_header(record[1]);
      movie_header.append("#@");
      movie_header.append(record[2]);

      /* if the movie_header not exist in the table */
      if ( moviesTable.count(movie_header) == 0 ) {
        vector<string> cast;
        cast.push_back(record[0]);
        moviesTable[movie_header] = cast;
      }
      /* if the movie_header exist in the table */
      else
        moviesTable[movie_header].push_back(record[0]);
    }// end of reading file

    /* create an ordered map based on the year */
    map<int, vector<movies*>> year_map;
    for ( auto it = moviesTable.begin(); it != moviesTable.end(); it++ ) {
      /* convert the year based on the header */
      unsigned int pos = it->first.size() - 4;
      string temp = it->first.substr(pos);
      int myYear = stoi(temp);
      /* if that year has not been matched */
      if ( year_map.count(myYear) == 0 ) {
        /* create a new movie structure */
        movies* myMovies = new movies();
        myMovies->year = myYear;
        myMovies->cast = it->second;
        /* push to the year table */
        vector<movies*> toPush;
        toPush.push_back(myMovies);
        year_map[myYear] = toPush;
      }
      /* if that year has been created */
      else {
        movies* myMovies = new movies();
        myMovies->year = myYear;
        myMovies->cast = it->second;
        year_map[myYear].push_back(myMovies);
      }
    }

    /* open the file to find */
    ifstream pair_file;
    pair_file.open(argv[2], ios::binary);
    ofstream output;
    output.open(argv[3], ofstream::out);

    /* if it is an invalid file */
    if ( !pair_file.is_open() ) {
      cout << "Failed to read " << argv[2] << "!\n";
      cout << argv[0] << " called with incorrect arguments.\n"
      << "Usage: " << argv[0] << " movie_cast_tsv_file pairs_tsv_file output_"
      << "cast_tsv_file ufind/bfs\n"
      << "If the last arguments if not specified the union find implementation"
      << "is used as default" << endl;
      return -1;
    }

    /* reading test pair file */
    bool myHeader = false;
    while (pair_file) {
      string ori_actor;
      /* get the next line */
      if ( !getline( pair_file, ori_actor ) )
        break;

      /* skip the header */
      if ( !myHeader ) {
        myHeader = true;
        output << "Actor1" << "\t" << "Actor2" << "\t" << "Year\n";
        continue;
      }

      istringstream bb(ori_actor);
      /* get the two actors */
      while ( bb ) {
        string actor1;
        string actor2;
        /* get the two actors devided by tab */
        if ( !getline(bb, actor1, '\t') )
          break;
        if ( !getline(bb, actor2, '\n') )
          break;

        /* compute the path using ufind */
        output << actor1 << "\t" << actor2 << "\t";
        int start_year = actorsTable[actor1]->first_year;
        vector<u_actor*> changed;
        auto it = year_map.begin();
        /* union the movies by the cast until the start_year */
        for(; it->first <= start_year; it++) {
          /* union the cast in one movie */
          for ( unsigned int i = 0; i < it->second.size(); i++ ) {
            //cout << it->second[i]->cast[0] << endl;
            //cout << it->second[i]->cast.size() << endl;
            changed.push_back(actorsTable[it->second[i]->cast[0]]);
            //if ( it->second[i]->cast.size() == 1 )
              //continue;
            for ( unsigned int j = 1; j < it->second[i]->cast.size(); j++ ) {
              merge(actorsTable[it->second[i]->cast[0]], 
                  actorsTable[it->second[i]->cast[j]]);
              changed.push_back(actorsTable[it->second[i]->cast[j]]);
            }
          }
        }// end of start_year union
        /* check from start year, whether they have connection */
        if ( find(actorsTable[actor1]) == find(actorsTable[actor2]) ) {
          output << start_year << endl;
          /* changed the graph to the original */
          for ( unsigned int h = 0; h < changed.size(); h++ ) {
            changed[h]->parent = nullptr;
            changed[h]->weight = 1;
          }
          continue;
        }
        bool found = false;
        /* continue adding edges based on years */
        for(; it != year_map.end(); it++) {
          /* union the cast in one movie */
          for ( unsigned int i = 0; i < it->second.size(); i++ ) {
            changed.push_back(actorsTable[it->second[i]->cast[0]]);
            for ( unsigned int j = 1; j < it->second[i]->cast.size(); j++ ) {
              merge(actorsTable[it->second[i]->cast[0]], 
                  actorsTable[it->second[i]->cast[j]]);
              changed.push_back(actorsTable[it->second[i]->cast[j]]);
            }
          }
          /* check for each year, whether they have connection */
          if ( find(actorsTable[actor1]) == find(actorsTable[actor2]) ) {
            output << it->first << endl;
            found = true;
            /* changed the graph to the original */
            for ( unsigned int h = 0; h < changed.size(); h++ ) {
              changed[h]->parent = nullptr;
              changed[h]->weight = 1;
            }
            break;
          }
        }// end of continue union
        if ( !found )
          output << "9999" << endl;

      }
    }// end of reading pair

    /* close the file */
    in.close();
    pair_file.close();
    output.close();

  } // end of using union find

/**************************Up: union *** Down: BFS***************************/

  /* if using bfs */
  else {

    /* create timer object */
    Timer T;
    double time_duration;

    /* beging to measure time */
    T.begin_timer();

    /* open the file */
    ifstream in;
    in.open(argv[1], ios::binary);

    /*cout << "Reading " << argv[1] << " ..." << endl;*/

    /* if it is an invalid file */
    if ( !in.is_open() ) {
      cout << "Failed to read " << argv[1] << "!\n";
      cout << argv[0] << " called with incorrect arguments.\n"
      << "Usage: " << argv[0] << " movie_cast_tsv_file pairs_tsv_file output_"
      << "cast_tsv_file ufind/bfs\n"
      << "If the last arguments if not specified the union find implementation"
      << "is used as default" << endl;
      return -1;
    }

    bool have_header = false;

    unordered_map<string, vector<string>> moviesTable;
    unordered_map<string, actors*> actorsTable;
    int num_edge = 0;

    /* keep reading lines until the end of file is reached */
    while ( in ) {

      string myString;

      /* get the next line */
      if ( !getline( in, myString ) )
        break;

      /* skip the header */
      if ( !have_header ) {
        have_header = true;
        continue;
      }

      istringstream ss(myString);
      vector<string> record;

      /* get the next string before hitting a tab character and put in next */
      while ( ss ) {
        string next;
        if ( !getline( ss, next, '\t' ) )
          break;
        record.push_back( next );
      }

      /* we should have exactly 3 columns */
      if ( record.size() != 3 )
        continue;

      /* if the actors not exist in the table */
      if ( actorsTable.count(record[0]) == 0 ) {
        actors* myActor = new actors();
        myActor->actor_name = record[0];
        actorsTable[record[0]] = myActor;
      }

      /* create a movie_header string */
      string movie_header(record[1]);
      movie_header.append("#@");
      movie_header.append(record[2]);

      /* if the movie_header not exist in the table */
      if ( moviesTable.count(movie_header) == 0 ) {
        vector<string> cast;
        cast.push_back(record[0]);
        moviesTable[movie_header] = cast;
      }
      /* if the movie_header exist in the table */
      else
        moviesTable[movie_header].push_back(record[0]);
    }// end of reading file

    /* update the connections between actors */
    for ( auto it = moviesTable.cbegin(); it != moviesTable.cend(); ++it ) {
      /* if there is only one actor in that movie */
      if ( it->second.size() == 1 )
        continue;
      for ( unsigned int i = 0; i < it->second.size(); i++ ) {
        for( unsigned int j = 0; j < it->second.size(); j++ ) {
          /* if i is less than j */
          if( i < j ) {
            /* create a new edge and update them into adjacency list */
            Edge* e = new Edge();
            e->left = actorsTable[it->second[i]];
            e->right = actorsTable[it->second[j]];
            /* get the year of the movie */
            string movie_year = it->first;
            unsigned int pos = movie_year.size() - 4;
            string nextString = movie_year.substr(pos);
            e->year = stoi(nextString);
            if ( e->year < actorsTable[it->second[i]]->first_year )
              actorsTable[it->second[i]]->first_year = e->year;
            actorsTable[it->second[i]]->adj.push_back(e);
            num_edge++;
          }
          /* if j is less than i */
          else if ( i > j ) {
            /* create a new edge and update them into adjacency list */
            Edge* e = new Edge();
            e->left = actorsTable[it->second[i]];
            e->right = actorsTable[it->second[j]];
            /* get the year of the movie */
            string movie_year = it->first;
            unsigned int pos = movie_year.size() - 4;
            string nextString = movie_year.substr(pos);
            e->year = stoi(nextString);
            if ( e->year < actorsTable[it->second[i]]->first_year )
              actorsTable[it->second[i]]->first_year = e->year;
            actorsTable[it->second[i]]->adj.push_back(e);
            num_edge++;
          }
        }
      }//end of second inner for
    }// outer inner for


    /* print information about the graph into the terminal*/
    /*cout << "#nodes: " << actorsTable.size() << endl;
    cout << "#movies: " << moviesTable.size() << endl;
    cout << "#edges: " << num_edge << endl;
    cout << "done" << endl;
    cout << endl << " Running BFS" << endl << endl;*/

    /* open the file to find */
    ifstream pair_file;
    pair_file.open(argv[2], ios::binary);
    ofstream output;
    output.open(argv[3], ofstream::out);

    /* if it is an invalid file */
    if ( !pair_file.is_open() ) {
      cout << "Failed to read " << argv[2] << "!\n";
      cout << argv[0] << " called with incorrect arguments.\n"
      << "Usage: " << argv[0] << " movie_cast_tsv_file pairs_tsv_file output_"
      << "cast_tsv_file ufind/bfs\n"
      << "If the last arguments if not specified the union find implementation"
      << "is used as default" << endl;
      return -1;
    }


    /* reading test pair file */
    bool myHeader = false;
    while (pair_file) {
      string ori_actor;
      /* get the next line */
      if ( !getline( pair_file, ori_actor ) )
        break;

      /* skip the header */
      if ( !myHeader ) {
        myHeader = true;
        output << "Actor1" << "\t" << "Actor2" << "\t" << "Year\n";
        continue;
      }

      istringstream bb(ori_actor);
      /* get the two actors */
      while ( bb ) {
        string actor1;
        string actor2;
        /* get the two actors devided by tab */
        if ( !getline(bb, actor1, '\t') )
          break;
        if ( !getline(bb, actor2, '\n') )
          break;

        /* compute the path using BFS */
        actors* struct1 = actorsTable[actor1];
        output << actor1 << "\t" << actor2 << "\t";
        BFS( struct1, actor2, output );
      }
    }// end of reading pair
    
    /* end timer */
    time_duration = T.end_timer();
    cout << "The duration in milliseconds is: " << time_duration << endl;
    
    /* close the file */
    in.close();
    pair_file.close();
    output.close();

    /* delete the heap memory */
    for ( auto it = actorsTable.begin(); it != actorsTable.end(); ++it ) {
      for ( unsigned int i = 0; i < it->second->adj.size(); i++ ) {
        delete it->second->adj[i];
      }
      delete it->second;
    }


  }// end of using bfs


  return 0;
}
