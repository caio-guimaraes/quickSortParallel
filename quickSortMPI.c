#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

// Program 10.1.
// The HyperQuickSort Method

// Declara  Variáveis globais
void ParallelHyperQuickSort ( double *pProcData, int ProcDataSize);
void PivotDistribution (double *pProcData, int ProcDataSize, int Dim, int Mask, int Iter, double *pPivot);
void ProcessInitialization (double *pProcData, int *ProcDataSize);
void imprimiVetor(int vet[], int tamanho);
int GetProcDataDivisionPos
int ProcRank; // ID do processo executando
int ProcNum; // Número total de processos



int main(int argc, char *argv[]) {
  double *pProcData; // Data block for the process
  int ProcDataSize; // Data block size

  // Inicializa variáveis MPI 
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
  MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);

  // Data Initialization and their distribution among the processors
  ProcessInitialization ( &pProcData, &ProcDataSize);

  // Parallel sorting
  ParallelHyperQuickSort ( pProcData, ProcDataSize );

  // The termination of process computations
  ProcessTermination ( pProcData, ProcDataSize );

  MPI_Finalize();
}

void ProcessInitialization (double *pProcData, int *ProcDataSize){
  int n = 4;
  pProcData =(double*) malloc(n*sizeof(double));
  *ProcDataSize = n / ProcNum;

  for(int i=0; i<n; i++){
    scanf("%d ", pProcData[i]);
  }

  imprimiVetor(pProcData, n);
}

void imprimiVetor(int vet[], int tamanho){
    int i;
    for (i = 0; i < tamanho; i++)
        printf("%d ", vet[i]);
    printf("\n");
}

// The Parallel HyperQuickSort Method
void ParallelHyperQuickSort ( double *pProcData, int ProcDataSize) {
  MPI_Status status;

  int CommProcRank; // Rank of the processor involved in communications
  double *pMergeData, // Block obtained after merging the block parts
  *pData, // Block part, which remains on the processor
  *pSendData, // Block part, which is sent to the processor CommProcRank
  *pRecvData; // Block part, which is received from the proc CommProcRank

  int DataSize, SendDataSize, RecvDataSize, MergeDataSize;
  int HypercubeDim = (int)ceil(log(ProcNum)/log(2)); // Hypercube dimension
  int Mask = ProcNum;
  double Pivot;

  // Local data sorting
  LocalDataSort ( pProcData, ProcDataSize );

  // Hyperquicksort iterations
  for ( int i = HypercubeDim; i > 0; i-- ) {
    // Determination of the pivot value and broadcast it to processors
    PivotDistribution (pProcData,ProcDataSize,HypercubeDim,Mask,i,&Pivot);
    Mask = Mask >> 1;

    // Determination of the data division position
    int pos = GetProcDataDivisionPos (pProcData, ProcDataSize, Pivot);

    // Block division
    if (((rank&Mask) >> (i-1)) == 0) { // high order bit= 0
      pSendData = & pProcData[pos+1];
      SendDataSize = ProcDataSize - pos – 1;

      if ( SendDataSize < 0 )
        SendDataSize = 0;

      CommProcRank = ProcRank + Mask
      pData = & pProcData[0];
      DataSize = pos + 1;
    }
    else { // high order bit = 1
      pSendData = & pProcData[0];
      SendDataSize = pos + 1;

      if ( SendDataSize > ProcDataSize )
        SendDataSize = pos;

      CommProcRank = ProcRank – Mask
      pData = & pProcData[pos+1];
      DataSize = ProcDataSize - pos - 1;

      if ( DataSize < 0 )
        DataSize = 0;
    }

    // Sending the sizes of the data block parts
    MPI_Sendrecv(&SendDataSize, 1, MPI_INT, CommProcRank, 0, &RecvDataSize, 1, MPI_INT, CommProcRank, 0, MPI_COMM_WORLD, &status);

    // Sending the data block parts
    pRecvData = new double[RecvDataSize];
    MPI_Sendrecv(pSendData, SendDataSize, MPI_DOUBLE,
    CommProcRank, 0, pRecvData, RecvDataSize, MPI_DOUBLE,
    CommProcRank, 0, MPI_COMM_WORLD, &status);

    // Data merge
    MergeDataSize = DataSize + RecvDataSize;
    pMergeData = new double[MergeDataSize];
    DataMerge(pMergeData, pMergeData, pData, DataSize, pRecvData, RecvDataSize);
    delete [] pProcData;
    delete [] pRecvData;
    pProcData = pMergeData;
    ProcDataSize = MergeDataSize;
  }
}

// Determination of the pivot value and broadcast it to all the processors
void PivotDistribution (double *pProcData, int ProcDataSize, int Dim, int Mask, int Iter, double *pPivot) {
  MPI_Group WorldGroup;
  MPI_Group SubcubeGroup; // a group of processors – a subhypercube
  MPI_Comm SubcubeComm; // subhypercube communcator
  int j = 0;

  int GroupNum = ProcNum /(int)pow(2, Dim-Iter);
  int *ProcRanks = new int [GroupNum];

  // Forming the list of ranks for the hypercube processes
  int StartProc = ProcRank – GroupNum;
  if (StartProc < 0 )
    StartProc = 0;

  int EndProc = (ProcRank + GroupNum;
  if (EndProc > ProcNum )
    EndProc = ProcNum;

  for (int proc = StartProc; proc < EndProc; proc++) {
    if ((ProcRank & Mask)>>(Iter) == (proc & Mask)>>(Iter)) {
      ProcRanks[j++] = proc;
    }
  }

  // Creating the communicator for the subhypercube processes
  MPI_Comm_group(MPI_COMM_WORLD, &WorldGroup);
  MPI_Group_incl(WorldGroup, GroupNum, ProcRanks, &SubcubeGroup);
  MPI_Comm_create(MPI_COMM_WORLD, SubcubeGroup, &SubcubeComm);

  // Selecting the pivot element and seding it to the subhypercube processes
  if (ProcRank == ProcRanks[0])
    *pPivot = pProcData[(ProcDataSize)/2];

  MPI_Bcast ( pPivot, 1, MPI_DOUBLE, 0, SubcubeComm );
  MPI_Group_free(&SubcubeGroup);
  MPI_Comm_free(&SubcubeComm);
  delete [] ProcRanks;
}
