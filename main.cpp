# include <iostream>
# include <ctime>
# include <fstream>
# include <string>
# include <vector>
# include <pthread.h> // 引用 pthread 函式庫
# include <sys/types.h>
# include <unistd.h>

# define LINESIZE 100
using namespace std;

fstream fin;
string inputName, outputName;

class NumVector {
public:
  vector<int> numVector;
}; // class Ptr

class Index {
public:
  int index;
  int max;

  Index() {
    index = max = 0;
  } // Index()

  Index( int _max ) {
    index = 0;
    max = _max;
  }
}; // class Index

vector< vector<int> > kNumVector;

void PrintNumVector( vector<int> numVector ) {
  // cout << "排序: " << endl;
  // for ( int i = 0 ; i < numVector.size() ; i++ )
  //   cout << numVector[i] << " ";
  // cout << endl;

  fin << "排序: " << endl;
  for ( int i = 0 ; i < numVector.size() ; i++ )
    fin << numVector[i] << " ";
  fin << endl;

} // PrintNumVector()

void WriteNumVector( vector<int> numVector, int num ) {
  fin.open( "temp" + to_string(num) + ".txt", ios::out | ios::trunc );
  for ( int i = 0 ; i < numVector.size() ; i++ )
    fin << numVector[i] << " ";
  fin.close();
} // WriteNumVector()

vector<int> ReadNumVector( int num ) {
  char line[LINESIZE];
  vector<int> numVector;
  fin.open( "temp" + to_string(num) + ".txt", ios::in );
  while ( fin.getline( line, sizeof( line ), ' ' ) ) {
    numVector.push_back( atoi( line ) );
  } // while
  fin.close();
  return numVector;
} // ReadNumVector()

void BubbleSort( vector<int> &numVector ) {
  for ( int i = 0 ; i < numVector.size() ; i++ ) {
    for ( int j = i + 1 ; j < numVector.size() ; j++ ) {
      if ( numVector[i] > numVector[j] ) {
        int num = numVector[i];
        numVector[i] = numVector[j];
        numVector[j] = num;
      } // if
    } // for
  } // for

} // BubbleSort()

vector<int> KWayMerge( int totalNum ) {
  vector<int> ansVector;
  vector<Index> indexVector;
  for ( int i = 0 ; i < kNumVector.size() ; i++ )
    indexVector.push_back( Index( kNumVector[i].size() ) );

  int count = 0;
  while ( count < totalNum ) {
    int minNum[2] = {0}; // 0 => minNum, 1 => indexOfMinNum
    minNum[1] = -1;
    for ( int i = 0 ; i < kNumVector.size() && minNum[1] == -1 ; i++ ) {
      if ( indexVector[i].index < indexVector[i].max ) {
        minNum[0] = kNumVector[i][ indexVector[i].index ];
        minNum[1] = i; // index of indexVector
      } // if
    } // for

    for ( int i = 0 ; i < kNumVector.size() ; i++ ) {
      if ( indexVector[i].index < indexVector[i].max)
        if ( kNumVector[i][ indexVector[i].index ] < minNum[0] ) {
          minNum[0] = kNumVector[i][ indexVector[i].index ];
          minNum[1] = i;
        } // if
    } // for

    ansVector.push_back( minNum[0] );
    indexVector[ minNum[1] ].index++;

    count++;
  } // while

  return ansVector;
} // KWayMerge()

void *BubbleSort_subF( void *arg ) {
  int i = *(int*)arg;
  BubbleSort( kNumVector[i] );

  return NULL;
} // BubbleSort_subF()

void MergeSort( vector<int> &numVector, int type ) {
  int kWay = 0, aWayLength = 0;
  cout << "請輸入要切成幾份: ";
  cin >> kWay;
  aWayLength = numVector.size() / kWay;

  int nowWay = 0;
  for ( int i = 0 ; i < kWay - 1 ; i++ ) {
    kNumVector.push_back( vector<int>( numVector.begin() + nowWay, numVector.begin() + nowWay + aWayLength ) );
    nowWay += aWayLength;
  } // for

  kNumVector.push_back( vector<int>( numVector.begin() + nowWay, numVector.end() ) );

  if ( type == 2 ) {
    pthread_t thread[kWay];
    for ( int i = 0 ; i < kWay ; i++ ) {
      int *n = new int(i);
      pthread_create(&thread[i], NULL, BubbleSort_subF, (void*)n);
    } // for

    for ( int i = 0 ; i < kWay ; i++ )
      pthread_join(thread[i], NULL);
  } // if
  if ( type == 3 ) {
    pid_t childPID[kWay];

    for ( int i = 0 ; i < kWay ; i++ ) {
      childPID[kWay] = fork();
      if ( childPID[kWay] == 0 ) {
        int *n = new int( i );
        BubbleSort_subF( (void*)n );
        WriteNumVector( kNumVector[*n], *n );
        exit(1);
      } // if
    } // for

    for ( int i = 0 ; i < kWay ; i++ ) {
      waitpid( childPID[i], NULL, 0 );
      kNumVector[i] = ReadNumVector( i );
      // for ( int j = 0 ; j < kNumVector[i].size() ; j++ )
      //   cout << kNumVector[i][j] << " ";
      // cout << endl;
    } // for

    // for ( int i = 0 ; i < kWay ; i++ )
    //   PrintNumVector( kNumVector[i] );
  } // if
  else if ( type == 4 ) {
    for ( int i = 0 ; i < kWay ; i++ ) {
      int *n = new int(i);
      BubbleSort_subF( (void*)n );
    } // for
  } // if

  // /////////////////////////// 以上分割 + BubbleSort

  // for ( int i = 0 ; i < kWay ; i++ )
  //   PrintNumVector( kNumVector[i] );
  numVector = KWayMerge( numVector.size() );

} // MergeSort()

void KWayMerge_thread( vector<int> numVector ) {
  int k = 10;
  pthread_t thread[k];
  pthread_create(&thread[0], NULL, BubbleSort_subF, NULL);
  pthread_join(thread[0], NULL);
} // KWayMerge_thread()

void Run( vector<int> numVector, int type ) {

  // type 1
  if ( type == 1 ) {
    BubbleSort( numVector );
    // outputName = "BubbleSort" + outputName;
    fin.open( outputName, ios::out | ios::trunc );
    PrintNumVector( numVector );
  } // if

  // type 2
  else if ( type == 2 ) {
    MergeSort( numVector, 2 );
    // outputName = "MergeSort_thread" + outputName;
    fin.open( outputName, ios::out | ios::trunc );
    PrintNumVector( numVector );
  } // else if

  // type 3
  else if ( type == 3 ) {
    MergeSort( numVector, 3 );
    // outputName = "MergeSort_process" + outputName;
    fin.open( outputName, ios::out | ios::trunc );
    PrintNumVector( numVector );
  } // else if

  // type 4
  else if ( type == 4 ) {
    MergeSort( numVector, 4 );
    // outputName = "MergeSort" + outputName;
    fin.open( outputName, ios::out | ios::trunc );
    PrintNumVector( numVector );
  } // else if

} // Run()

int main() {
  char line[LINESIZE];
  int indexOfDot = 0, type = 0;
  vector<int> numVector;

  cout << "請輸入檔案名稱: ";
  cin >> inputName;
  indexOfDot = inputName.find( "." );
  if ( indexOfDot > -1 )
    outputName = inputName.substr( 0, inputName.length() - 4 ) + "_output.txt";
  else
    outputName = "output.txt";

  fin.open( inputName, ios::in );
  fin.getline( line, sizeof( line ), '\n' );
  cout << line << endl;
  type = atoi( line );
  while ( fin.getline( line, sizeof( line ), ' ' ) ) {
    numVector.push_back( atoi( line ) );
  } // while

  fin.close();

  // numVector = vector<int>(numVector.begin(), numVector.begin() + numVector.size() / 2);
  // numVector = vector<int>(numVector.begin(), numVector.begin() + numVector.size() / 2);
  // numVector = vector<int>(numVector.begin(), numVector.begin() + numVector.size() / 2);

  clock_t start = clock();
  // 宣稱資料型態為 clock_t 的 start 變數; clock() 取得 CPU 目前的clock

  // want calculate
  Run( numVector, type );
  // want calculate

  clock_t end = clock(); // 宣稱資料型態為 clock_t 的 end 變數
  float costTime = (float)(end - start)/CLOCKS_PER_SEC; // 求算CPU執行時間
  // cout << "執行時間: " << costTime;
  fin << "執行時間: " << costTime;
  fin.close();
  return 0;
} // main()
