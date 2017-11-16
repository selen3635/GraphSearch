/*
 * Name: Ziying Hu & Xiaolong Zhou
 * Login: cs100vat & cs100vcc
 * Date: Aug 26, 2016
 * File: pathfinder.cpp
 */

#include <iostream>
#include <climits>
#include <vector>
#include <queue>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>


using namespace std;
struct Edge;
struct actors;

/* the struct for actor nodes */
struct actors
{
  // The name of actor */
  string actor_name;

  // The adjacency list
  vector<Edge*> adj;

  // The distance from the source
  int dist = INT_MAX;

  // The index of the vertex previous in the path
  Edge* prev = nullptr;

  bool done = false;
};

/* the struct for edge */
struct Edge {
  string movie_year;
  actors* left;
  actors* right;
  int weight;
};

class DistanceComp {

  public:
    bool operator()(pair<actors*, int>& lhs, pair<actors*, int>& rhs) const {
      return lhs.second > rhs.second;
    }
};



/********************************BFS traverse *******************************/

/** Traverse the graph using BFS */
void BFSTraverse( struct actors* actor1, string toFind, ofstream& out) {
  /* the structrure to store the vertex */
  queue<actors*> toExplore;
  vector<actors*> changed;
  actors* start = actor1;
  start->dist = 0;
  changed.push_back(start);
  toExplore.push(start);

  /* if the queue is not empty, still doing BST */
  while( !toExplore.empty() ) {

    /* pop the first node */
    actors* next = toExplore.front();
    toExplore.pop();
    
    /* traverse the adjacency list */
    for( unsigned int j = 0; j < next->adj.size(); ++j ) {
      /* update the neighbor according to edge */
      actors* neighbor;
      if ( next->adj[j]->left == next )
        neighbor = next->adj[j]->right;
      else
        neighbor = next->adj[j]->left;
      /* update the distance if it is smaller */
      if( next->dist + 1 < neighbor->dist ) {
        neighbor->dist = next->dist + 1;
        neighbor->prev = next->adj[j];
        changed.push_back(neighbor);
        toExplore.push(neighbor);

        /* if find the path */
        if ( toFind.compare(neighbor->actor_name) == 0 ) {
          /* to print the path */
          vector<string> toPrint;
          /* traversing the path backwards */
          while( neighbor->prev != 0 ) {
            toPrint.push_back ( neighbor->actor_name );
            Edge* e = neighbor->prev;
            toPrint.push_back( e->movie_year );
            if ( e->left == neighbor )
              neighbor = e->right;
            else
              neighbor = e->left;
          }
          toPrint.push_back(neighbor->actor_name);
          /* print the path to the output file */
          for ( unsigned int i = toPrint.size() - 1; (int)i >= 0; i-- ) {
            if ( i % 2 == 0 )
              out << "(" << toPrint[i] << ")";
            else 
              out << "--[" << toPrint[i] << "]-->";
          }
          out << "\n";

          /* change the changed actors dist and prev into original */
          for ( unsigned int i = 0; i < changed.size(); i++ ) {
            changed[i]->dist = INT_MAX;
            changed[i]->prev = nullptr;
          }
          return;
        }
      }

    }//end of for

  }//end of while
}

/**************************** Dijkstra's Algorithm **************************/

/** Traverse the graph using BFS */
void DTraverse( pair<actors*, int> myPair , string toFind, ofstream& out) {
  /* the structrure to store the vertex */
  priority_queue<pair<actors*, int>, vector<pair<actors*, int>>, 
    DistanceComp> myQ;
  vector<actors*> changed;
  myPair.first->dist = 0;
  changed.push_back(myPair.first);
  myQ.push(myPair);
  actors* startpoint = nullptr;

  /* if the queue is not empty, still doing BST */
  while( !myQ.empty() ) {

    /* pop the first node */
    pair<actors*, int> next =  myQ.top();

    if( toFind.compare(next.first->actor_name) == 0 ){
      startpoint = next.first;
      break;
    }
    myQ.pop();
   
    /* check whether node is done or not */
    if( !(next.first->done) ) {

      /* set done to true */
      next.first->done = true;

      /* traverse the adjacency list */
      for( unsigned int i = 0; i < next.first->adj.size(); ++i ) {

        /* update the neighbor according to edge */
        actors* neighbor;
        if( next.first->adj[i]->left == next.first)
          neighbor = next.first->adj[i]->right;
        else
          neighbor = next.first->adj[i]->left;

        /* update the distance */
        int myDist = next.first->dist + next.first->adj[i]->weight;

        /* check whether have less distance */
        if( myDist < neighbor->dist ) {

          /* update pre edge and distance */
          neighbor->prev = next.first->adj[i];
          neighbor->dist = myDist;

          changed.push_back(neighbor);

          pair<actors*, int> newPair( neighbor, myDist);
          myQ.push(newPair);

        }//end of inner if

      }//end of for

    }// end of if

  }//end of while

  /* to print the path */
  vector<string> toPrint;
  /* traversing the path backwards */
  while( startpoint->prev != 0 ) {
    toPrint.push_back ( startpoint->actor_name );
    Edge* e = startpoint->prev;
    toPrint.push_back( e->movie_year );
    if ( e->left == startpoint )
      startpoint = e->right;
    else
      startpoint = e->left;
  }
  toPrint.push_back(startpoint->actor_name);
  
  /* print the path to the output file */
  for ( unsigned int i = toPrint.size() - 1; (int)i >= 0; i-- ) {
    if ( i % 2 == 0 )
      out << "(" << toPrint[i] << ")";
    else 
      out << "--[" << toPrint[i] << "]-->";
  }
  out << "\n";

  /* change the changed actors dist and prev into original */
  for ( unsigned int i = 0; i < changed.size(); i++ ) {
    changed[i]->dist = INT_MAX;
    changed[i]->prev = nullptr;
    changed[i]->done = false;
  }

}//end of function


/***************************************************************************/



/*
 * Main driver of the program
 */
int main( int argc, char* argv[] ) {


  int num_edge = 0;

  /* check for argument */
  if ( argc != 5 ) {

    cout << "Usage: ./" << argv[0] <<" movie_cast_tsv_file u/w pairs_tsv_file"  
      << "output_paths_tsv_file" << endl;
    
    return -1;
  }


  string argv2 = argv[2];
  /* if the user asked to find in unweighted graph */
  if ( argv2 == "u" ) {

    /* open the file */
    ifstream in;
    in.open(argv[1], ios::binary);

    cout << "Reading " << argv[1] << " ..." << endl;

    /* if it is a invalid file */
    if ( !in.is_open() ) {
      cout << "Failed to read " << argv[1] << "!\n" 
        << "./" << argv[0] << " called with incorrect arguments.\n"
        << "Usage: ./" << argv[0] <<" movie_cast_tsv_file u/w pairs_tsv_file"  
        << "output_paths_tsv_file" << endl;
      return -1;
    }

    bool have_header = false;

    unordered_map<string, vector<string>> moviesTable;
    unordered_map<string, actors*> actorsTable;

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
      vector < string > record;

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

    }// the end of while reading loop

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
            e->movie_year = it->first;
            actorsTable[it->second[i]]->adj.push_back(e);
            num_edge++;
          }
          /* if j is less than i */
          else if ( i > j ) {
            /* create a new edge and update them into adjacency list */
            Edge* e = new Edge();
            e->left = actorsTable[it->second[i]];
            e->right = actorsTable[it->second[j]];
            e->movie_year = it->first;
            actorsTable[it->second[i]]->adj.push_back(e);
            num_edge++;
          }
        }
      }//end of second inner for
    }// outer inner for


    /* print information about the graph into the terminal*/
    cout << "#nodes: " << actorsTable.size() << endl;
    cout << "#movies: " << moviesTable.size() << endl;
    cout << "#edges: " << num_edge << endl;
    cout << "done" << endl;

    /* open the file to find */
    ifstream pair;
    pair.open(argv[3], ios::binary);
    ofstream output;
    output.open(argv[4], ofstream::out);

    /* if it is a invalid file */
    if ( !pair.is_open() ) {
      cout << "Failed to read " << argv[3] << "!\n" 
        << "./" << argv[0] << " called with incorrect arguments.\n"
        << "Usage: ./" << argv[0] <<" movie_cast_tsv_file u/w pairs_tsv_file"  
        << "output_paths_tsv_file" << endl;
      return -1;
    }

    bool myHeader = false;
    while(pair) {
      string ori_actor;
      /* get the next line */
      if ( !getline( pair, ori_actor ) )
        break;

      /* skip the header */
      if ( !myHeader ) {
        myHeader = true;
        output << "(actor)--[movie#@year]-->(actor)--...\n";
        continue;
      }

      istringstream bb(ori_actor);
      /* get the two actors */
      while( bb ) {
        string actor1;
        string actor2;
        /* get the two actors devided by tab */
        if( !getline( bb, actor1, '\t') )
          break;
        if( !getline( bb, actor2, '\n') )
          break;

        /* compute the path used BFS */
        cout << "Computing path for (" << actor1 << ") -> (" << actor2 
          << ")" << endl;
        actors* struct1 = actorsTable[actor1];
        BFSTraverse( struct1, actor2, output); 
      }//end of inner while
    }//end of while

    /* close the file */
    in.close();
    pair.close();
    output.close();   

    /* delete the heap memory */
    for ( auto it = actorsTable.begin(); it != actorsTable.end(); ++it ) {
      for ( unsigned int i = 0; i < it->second->adj.size(); i++ ) {
        delete it->second->adj[i];
      }
      delete it->second;
    }
  }

/************************************************************************/
  /* if the user asked to find in weighed graph */
  else if ( argv2 == "w" ) {

    /* open the file */
    ifstream in;
    in.open(argv[1], ios::binary);

    cout << "Reading " << argv[1] << " ..." << endl;

    /* if it is a invalid file */
    if ( !in.is_open() ) {
      cout << "Failed to read " << argv[1] << "!\n" 
        << "./" << argv[0] << " called with incorrect arguments.\n"
        << "Usage: ./" << argv[0] <<" movie_cast_tsv_file u/w pairs_tsv_file"  
        << "output_paths_tsv_file" << endl;
      return -1;
    }

    bool have_header = false;

    unordered_map<string, vector<string>> moviesTable;
    unordered_map<string, actors*> actorsTable;

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
      vector < string > record;

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

    }// the end of while reading loop

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
            e->movie_year = it->first;

            /* get year of the movie */
            string nextString = "";
            unsigned int pos = e->movie_year.size() - 4;
            nextString = e->movie_year.substr(pos);
            int myYear = stoi(nextString);

            /* update weight of the edge */
            e->weight = 1 + (2015 - myYear);
         
            actorsTable[it->second[i]]->adj.push_back(e);  
            num_edge++;
          }
          /* if j is less than i */
          else if ( i > j ) {
            /* create a new edge and update them into adjacency list */
            Edge* e = new Edge();
            e->left = actorsTable[it->second[i]];
            e->right = actorsTable[it->second[j]];
            e->movie_year = it->first;

            /* get year of the movie */
            string nextString = "";
            unsigned int pos = e->movie_year.size() - 4;
            nextString = e->movie_year.substr(pos);
            int myYear = stoi(nextString);

            /* update weight of the edge */
            e->weight = 1 + ( 2015 - myYear);

            actorsTable[it->second[i]]->adj.push_back(e);
            num_edge++;
          }
        }
      }//end of second inner for
    }// outer inner for

    /* print information about the graph into the terminal*/
    cout << "#nodes: " << actorsTable.size() << endl;
    cout << "#movies: " << moviesTable.size() << endl;
    cout << "#edges: " << num_edge << endl;
    cout << "done" << endl;


    /* open the file to find */
    ifstream pair;
    pair.open(argv[3], ios::binary);
    ofstream output;
    output.open(argv[4], ofstream::out);

    /* if it is a invalid file */
    if ( !pair.is_open() ) {
      cout << "Failed to read " << argv[3] << "!\n" 
        << "./" << argv[0] << " called with incorrect arguments.\n"
        << "Usage: ./" << argv[0] <<" movie_cast_tsv_file u/w pairs_tsv_file"  
        << "output_paths_tsv_file" << endl;
      return -1;
    }

    bool myHeader = false;
    while(pair) {
      string ori_actor;
      /* get the next line */
      if ( !getline( pair, ori_actor ) )
        break;

      /* skip the header */
      if ( !myHeader ) {
        myHeader = true;
        output << "(actor)--[movie#@year]-->(actor)--...\n";
        continue;
      }

      istringstream bb(ori_actor);
      /* get the two actors */
      while( bb ) {
        string actor1;
        string actor2;
        /* get the two actors devided by tab */
        if( !getline( bb, actor1, '\t') )
          break;
        if( !getline( bb, actor2, '\n') )
          break;

        /* compute the path used BFS */
        cout << "Computing path for (" << actor1 << ") -> (" << actor2 
          << ")" << endl;
        actors* struct1 = actorsTable[actor1];
        std::pair<struct actors*, int> myPair = make_pair(struct1, 0);
        DTraverse( myPair, actor2, output); 
      }//end of inner while
    }//end of while


    /* close the file */
    in.close();
    pair.close();
    output.close(); 


    /* delete the heap memory */
    for ( auto it = actorsTable.begin(); it != actorsTable.end(); ++it ) {
      for ( unsigned int i = 0; i < it->second->adj.size(); i++ ) {
        delete it->second->adj[i];
      }
      delete it->second;
    }

  } // end of else if, argument parsing 'w'

  /* else the user is entered a wrong parameter */
  else {
    cout << "wrong parameter '" << argv[2] << "', mush be u or w" << endl;
    return -1;
  }


  return 0;


}//end of function
