#include <bits/stdc++.h>

using namespace std;

typedef struct
{
  int f;
  int c;
  int n;
} Movement;

typedef struct
{
  bool eraseCol;
  bool eraseRow;
  bool eraseSqr;
  int f;
  int c;
  int n;
  vector<Movement> vm;
} Movimientos;

int **tablero;
int tam, sqTam;
int faltantes;
int inF, inC, inN;
set<int> **domainCasillas;
set<int> *filas, *columnas, *cuadros;

stack<Movimientos> moves;

void printResult();
void printDomain();
int getLowBound(int);
void ForwardChecking(int, int);
int getSq(int, int);
bool isCandidate(int, int, int);
void updateDomains(int, int, int);
bool applyForwardChild(int, int, int);
void removeForwardChild(int, int, int);

int main()
{
  clock_t inicio, fin;
  cin >> tam;

  sqTam = floor(sqrt(tam));
  faltantes = tam * tam;

  tablero = new int *[tam];
  domainCasillas = new set<int> *[tam];
  filas = new set<int>[tam];
  columnas = new set<int>[tam];
  cuadros = new set<int>[tam];

  for (int i = 0; i < tam; i++)
  {
    tablero[i] = new int[tam];
    domainCasillas[i] = new set<int>[tam];
    for (int j = 0; j < tam; j++)
    {
      tablero[i][j] = 0;
      for (int k = 1; k <= tam; k++)
      {
        filas[i].insert(k);
        columnas[i].insert(k);
        cuadros[i].insert(k);
        domainCasillas[i][j].insert(k);
      }
    }
  }

  do
  {
    cin >> inF >> inC >> inN;
    if (inF != 0 && inC != 0 && inN != 0)
    {
      tablero[inF - 1][inC - 1] = inN;
      updateDomains(inF - 1, inC - 1, inN);
      faltantes--;
    }
  } while (inF != 0 && inC != 0 && inN != 0);

  inicio = clock();
  ForwardChecking(0, 0);
  fin = clock();

  // inicio = clock();
  // ArcConsistency(0, 0);
  // fin = clock();

  return 0;
}

int getLowBound(int numb)
{
  int a = numb / sqTam;
  return a * sqTam;
}

void printResult()
{
  cout << "------------------------" << endl;
  for (int i = 0; i < tam; i++)
  {
    for (int j = 0; j < tam; j++)
    {
      cout << tablero[i][j] << '\t';
    }
    cout << endl;
  }
  cout << "------------------------" << endl;
}

void printDomain()
{
  for (int i = 0; i < tam; i++)
  {
    for (int j = 0; j < tam; j++)
    {
      cout << "I: " << i << ", J: " << j << " -> ";
      for (set<int>::iterator it = domainCasillas[i][j].begin(); it != domainCasillas[i][j].end(); it++)
      {
        cout << *it << " ";
      }
      cout << endl;
    }
  }
}

int getSq(int f, int c)
{
  int fOffset = (int)floor((float)f / (float)sqTam);
  int cOffset = (int)floor((float)c / (float)sqTam);
  return (fOffset * sqTam) + cOffset;
}

void updateDomains(int fil, int col, int num)
{
  int sf, sc;

  for (int i = 0; i < tam; i++)
  {
    if (i != col && domainCasillas[fil][i].find(num) != domainCasillas[fil][i].end())
    {
      domainCasillas[fil][i].erase(num);
    }
    if (i != fil && domainCasillas[i][col].find(num) != domainCasillas[i][col].end())
    {
      domainCasillas[i][col].erase(num);
    }
  }

  filas[fil].erase(num);
  columnas[col].erase(num);
  cuadros[getSq(fil, col)].erase(num);

  sf = getLowBound(fil);
  sc = getLowBound(col);

  for (int i = 0; i < sqTam; i++)
  {
    for (int j = 0; j < sqTam; j++)
    {
      if (tablero[sf + i][sc + j] == 0 && domainCasillas[sf + i][sc + j].find(num) != domainCasillas[sf + i][sc + j].end())
      {
        domainCasillas[sf + i][sc + j].erase(num);
      }
    }
  }

  domainCasillas[fil][col].clear();
  domainCasillas[fil][col].insert(num);
}

bool isCandidate(int f, int c, int n)
{
  int sq = getSq(f, c);
  return filas[f].find(n) != filas[f].end() && columnas[c].find(n) != columnas[c].end() && cuadros[sq].find(n) != cuadros[sq].end();
}

void ForwardChecking(int fila, int columna)
{
  if (fila >= tam)
  {
    printResult();
  }
  else if (tablero[fila][columna] != 0)
  {
    if ((columna + 1) % tam == 0)
    {
      ForwardChecking(fila + 1, 0);
    }
    else
    {
      ForwardChecking(fila, columna + 1);
    }
  }
  else
  {
    for (set<int>::iterator it = domainCasillas[fila][columna].begin(); it != domainCasillas[fila][columna].end(); it++)
    {
      if (applyForwardChild(fila, columna, (*it)))
      {
        if ((columna + 1) % tam == 0)
        {
          ForwardChecking(fila + 1, 0);
        }
        else
        {
          ForwardChecking(fila, columna + 1);
        }
      }
      removeForwardChild(fila, columna, (*it));
    }
  }
}

bool applyForwardChild(int fila, int column, int nmb)
{
  bool fl = true;
  int sf, sc;
  tablero[fila][column] = nmb;
  faltantes--;
  Movimientos mvs;

  mvs.f = fila;
  mvs.c = column;
  mvs.n = nmb;
  mvs.eraseCol = false;
  mvs.eraseRow = false;
  mvs.eraseSqr = false;

  for (int i = 0; i < tam && fl; i++)
  {
    Movement mf, mc;
    if (i != column && domainCasillas[fila][i].find(nmb) != domainCasillas[fila][i].end() && tablero[fila][i] == 0)
    {

      domainCasillas[fila][i].erase(nmb);
      mf.f = fila;
      mf.c = i;
      mf.n = nmb;
      mvs.vm.push_back(mf);

      if (domainCasillas[fila][i].size() == 0)
      {
        fl = false;
      }
    }
    if (i != fila && domainCasillas[i][column].find(nmb) != domainCasillas[i][column].end() && tablero[i][column] == 0)
    {

      domainCasillas[i][column].erase(nmb);
      mc.f = i;
      mc.c = column;
      mc.n = nmb;
      mvs.vm.push_back(mc);

      if (domainCasillas[i][column].size() == 0)
      {
        fl = false;
      }
    }
  }

  sf = getLowBound(fila);
  sc = getLowBound(column);

  for (int i = 0; i < sqTam && fl; i++)
  {
    for (int j = 0; j < sqTam && fl; j++)
    {
      Movement msq;
      if (tablero[sf + i][sc + j] == 0 && domainCasillas[sf + i][sc + j].find(nmb) != domainCasillas[sf + i][sc + j].end() && !(sf + i == fila && sc + j == column))
      {
        domainCasillas[sf + i][sc + j].erase(nmb);
        msq.f = sf + i;
        msq.c = sc + j;
        msq.n = nmb;
        mvs.vm.push_back(msq);
        if (domainCasillas[sf + i][sc + j].size() == 0)
        {
          fl = false;
        }
      }
    }
  }

  if (filas[fila].find(nmb) != filas[fila].end())
  {
    filas[fila].erase(nmb);
    mvs.eraseRow = true;
  }
  else
  {
    fl = false;
  }
  if (columnas[column].find(nmb) != columnas[column].end())
  {
    columnas[column].erase(nmb);
    mvs.eraseCol = true;
  }
  else
  {
    fl = false;
  }
  if (cuadros[getSq(fila, column)].find(nmb) != cuadros[getSq(fila, column)].end())
  {
    cuadros[getSq(fila, column)].erase(nmb);
    mvs.eraseSqr = true;
  }
  else
  {
    fl = false;
  }

  moves.push(mvs);
  return fl;
}

void removeForwardChild(int f, int c, int n)
{
  Movimientos mvs;

  tablero[f][c] = 0;

  mvs = moves.top();
  moves.pop();

  if (mvs.eraseCol)
  {
    columnas[mvs.c].insert(mvs.n);
  }

  if (mvs.eraseRow)
  {
    filas[mvs.f].insert(mvs.n);
  }

  if (mvs.eraseCol)
  {
    cuadros[getSq(mvs.f, mvs.c)].insert(mvs.n);
  }

  for (vector<Movement>::iterator it = mvs.vm.begin(); it != mvs.vm.end(); it++)
  {
    domainCasillas[it->f][it->c].insert(it->n);
  }
}

// void ArcConsistency(int fil, int col)
// {
//   set<int>::iterator it;
//   int *options;
//   int tam = columnsDomain[searchCol].size();
//   int i = 0;

//   if (searchCol == total)
//   {
//     getSolution();
//   }
//   else
//   {
//     options = new int[tam];

//     for (it = columnsDomain[searchCol].begin(); it != columnsDomain[searchCol].end(); it++)
//     {
//       options[i] = *it;
//       i++;
//     }
//     for (i = 0; i < tam; i++)
//     {
//       applyArcChild(options[i], searchCol);
//       removeArcChild(options[i], searchCol);
//     }
//   }
// }