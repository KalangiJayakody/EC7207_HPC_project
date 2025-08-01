# EC7207 – HIGH PERFORMANCE COMPUTING
PROJECT REPORT
## Transaction Data Filtering and Analysis Based on User-Specified Query Parameters

Name     : JAYAKODY J.A.T.K.\
Reg No   : EG/2020/3978\
Semester : 07

## 1. Introduction

Large-scale financial datasets, such as bank transaction records, contain millions of entries that need to be filtered and analyzed efficiently. 
Traditional serial processing methods are not suitable for such large datasets as they result in high execution times, poor scalability reduce performance in terms of speed.

In this project, the objective was to implement parallel programming approaches to enhance the performance of the serial version. 
The goal was to evaluate the accuracy, execution time, and scalability of each parallel programming model in order to identify the most effective approach for large dataset filtering.

The code implements three types of parallel programming methods:
-  Shared memory programming (OpenMP)
-  Distributed memory programming (MPI)
-  Hybrid shared and distributed memory programming (OpenMP + MPI)

## 2. Results

The performance of each parallelization approach was measured using the following metrics.

- Execution Time: The time taken to filter the dataset.
- Accuracy: Whether the filtered results match those of the serial implementation.
- Scalability: How the performance improves as the number of threads or processes increases.

## 3. Serial Code Implementation

<div align="center">
  <figure>
    <div>
    <img width="336" height="460" alt="image" src="https://github.com/user-attachments/assets/cdf34847-fc51-42e9-a55e-e5befc6fc99d" />
    </div>
    <figcaption><i>Figure 1: Flow Diagram for the Serial Version</i></figcaption>
  </figure>
</div>
<br/>
Here, the code executes the query serially and retrieves the final output. The timer starts before the loop begins. 
Initially, the user is prompted to enter a location choice and the preferred gender. 
Then, the for loop iterates through a dynamic array of 1 million records, processing each transaction one by one. 
After the query completes, the timer stops, and the program prints the time taken to execute the loop serially.

## 4. Shared Memory (OpenMp) Code Implementation

<div align="center">
  <figure>
    <div>
    <img width="425" height="480" alt="image" src="https://github.com/user-attachments/assets/a2117a91-560f-443b-9ade-932455279153" />
    </div>
    <figcaption><i>Figure 2: Flow Diagram for the Shared Memory(OpenMp) Version</i></figcaption>
  </figure>
</div>
<br/>
In the OpenMP implementation, only the for loop was parallelized. The timer starts just before the parallel section begins and stops after 
the global count value is obtained. Different numbers of threads were tested during execution. Each thread processes a portion of the data 
and calculates a local count. These local counts are then summed using the reduction clause to produce the final result.

## 5.Distributed Memory (MPI) Code Implementation

<div align="center">
  <figure>
    <div>
    <img width="573" height="532" alt="image" src="https://github.com/user-attachments/assets/c1671e3d-25b2-47b0-ba78-f868642c02c4" />
    </div>
    <figcaption><i>Figure 3: Flow Diagram for the Distributed Memory(MPI) Version</i></figcaption>
  </figure>
</div>
<br/>
Here, the data paralleled using processes. Only rank 0 (master process) loads the data from the CSV file and collects all unique locations. 
The user is prompted to select a location and gender to filter the transactions. The selected filter values and total record count are 
broadcasted to all MPI processes using MPI_Bcast. The dataset is divided into chunks, and each process receives a portion of the data 
using MPI_Send and MPI_Recv. Each process applies the filter on its local chunk using the countMatching(). The local result 
(count of matching records) from each process is then reduced using MPI_Reduce to get the global result. The total execution 
time is measured using MPI_Wtime() before broadcasting and after the final reduction. Finally, only rank 0 prints the number 
of matching records and the total elapsed time.

## 6.Hybrid (OpenMp + MPI) Code Implementation

<div align="center">
  <figure>
    <div>
      <img width="834" height="513" alt="image" src="https://github.com/user-attachments/assets/ba477d9a-62d9-4874-9c08-c45dd436de4b" />
    </div>
    <figcaption><i>Figure 4: Flow Diagram for the Hybrid Version</i></figcaption>
  </figure>
</div>
<br/>

The process of the hybrid approach, as illustrated in the picture, is explained below.
- MPI (Distributed):
Rank 0 loads the entire dataset (transactions.csv) into memory. It extracts all unique locations and prompts the user to select a location and gender for filtering.
The user input and total record count are broadcast to all processes using MPI_Bcast. The dataset is divided into chunks and distributed across all MPI processes using MPI_Send and MPI_Recv.
- OpenMP (Shared):
Each process performs parallel filtering on its local chunk using OpenMP's #pragma omp parallel for with a reduction clause to count matching transactions
based on the selected filters. The OpenMP section is configured to run with 8 threads, but can be changed as needed.
- Final Aggregation:
Local counts from each MPI process are reduced into a final result using MPI_Reduce (sum operation).
The elapsed time is measured using MPI_Wtime() before broadcasting and after final reduction.

## 7. Performance Metrics Comparison

### 1. Accuracy

**Accuracy(%)** = (Matched Data_parallel / Matched Data_serial) × 100  
**RMSE** = √( (1/n) × Σ(yi - ŷi)² )

**Where:**  
- *yi* = Actual Data (Serial)  
- *ŷi* = Predicted Data (Parallel)  
- *n*  = Number of Comparisons

<p align="center"><strong>Table 1: Accuracy and RMSE Comparison Across Versions</strong></p>
<table align="center">
  <thead>
    <tr>
      <th>Version</th>
      <th>Matched Data</th>
      <th>Accuracy (%)</th>
      <th>RMSE</th>
    </tr>
  </thead>
  <tbody>
    <tr><td align="center">Serial</td><td align="center">50143</td><td align="center">-</td><td align="center">-</td></tr>
    <tr><td align="center">OpenMP - 4 threads</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">OpenMP - 8 threads</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">OpenMP - 12 threads</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">MPI - 2 processes</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">MPI - 4 processes</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">MPI - 8 processes</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">OpenMP+MPI 2p × 4t</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">OpenMP+MPI 2p × 8t</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">OpenMP+MPI 2p × 12t</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">OpenMP+MPI 4p × 4t</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">OpenMP+MPI 4p × 8t</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
    <tr><td align="center">OpenMP+MPI 4p × 12t</td><td align="center">50143</td><td align="center">100%</td><td align="center">0</td></tr>
  </tbody>
</table>


According to the table, it is clearly observed that the accuracy for each configuration is 100%, and the RMSE (Root Mean Square Error) is 0. 
This indicates that the results produced by all parallel implementations using OpenMP, MPI, and their combination are identical to the 
results obtained from the serial version. Therefore, we can conclude that parallel processing with varying parameters (threads and processes) 
does not affect the correctness of the output. This also implies that, for large datasets, parallel execution is a reliable alternative to 
serial processing, offering accurate results while improving performance.

### 2.Execution Time

**Speedup** = T<sub>Serial</sub> / T<sub>Parallel</sub>

<p align="center"><strong>Table 2: Speedup Comparison Across Versions</strong></p>

<table align="center">
  <thead>
    <tr>
      <th>Version</th>
      <th>Execution Time (sec)</th>
      <th>Speedup</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>Serial</td><td>0.0587</td><td>-</td></tr>
    <tr><td>OpenMP - 4 threads</td><td>0.0211</td><td>2.7820</td></tr>
    <tr><td>OpenMP - 8 threads</td><td>0.0164</td><td>3.5793</td></tr>
    <tr><td>OpenMP - 12 threads</td><td>0.0174</td><td>3.3736</td></tr>
    <tr><td>MPI - 2 processes</td><td>0.1210</td><td>0.4851</td></tr>
    <tr><td>MPI - 4 processes</td><td>0.1873</td><td>0.3134</td></tr>
    <tr><td>MPI - 8 processes</td><td>0.3098</td><td>0.1895</td></tr>
    <tr><td>OpenMP+MPI 2p × 4t</td><td>0.1073</td><td>0.5471</td></tr>
    <tr><td>OpenMP+MPI 2p × 8t</td><td>0.1179</td><td>0.4979</td></tr>
    <tr><td>OpenMP+MPI 2p × 12t</td><td>0.1377</td><td>0.4263</td></tr>
    <tr><td>OpenMP+MPI 4p × 4t</td><td>0.2215</td><td>0.2650</td></tr>
    <tr><td>OpenMP+MPI 4p × 8t</td><td>0.1633</td><td>0.3595</td></tr>
    <tr><td>OpenMP+MPI 4p × 12t</td><td>0.1707</td><td>0.3439</td></tr>
  </tbody>
</table>


According to the execution time and speedup table, it is evident that the OpenMP (shared memory) implementation achieves the highest 
speedup compared to the serial version, making it the most efficient in terms of performance. In contrast, the pure MPI 
(distributed memory) model has a lower speedup and performs slower than the serial version. This could be due to the overhead 
introduced by inter-process communication in MPI, especially when dealing with small datasets or insufficient computation per process.

However, in the hybrid model (MPI + OpenMP), we observe improved performance compared to MPI alone. 
This is likely because the OpenMP component reduces the internal processing time within each MPI process by 
using multithreading. While the hybrid approach doesn't go beyond the performance of pure OpenMP in this case, 
it still demonstrates better scalability than using MPI alone.

### 3.Scalability

- For Shared Memory

<div align="center">

  <table>
    <caption><strong>Table 3: Scalability Comparison in Shared Memory Version</strong></caption>
    <thead>
      <tr>
        <th>Version</th>
        <th>Execution Time (sec)</th>
      </tr>
    </thead>
    <tbody>
      <tr><td>Serial</td><td>0.0587</td></tr>
      <tr><td>OpenMP - 4 threads</td><td>0.0211</td></tr>
      <tr><td>OpenMP - 8 threads</td><td>0.0164</td></tr>
      <tr><td>OpenMP - 12 threads</td><td>0.0174</td></tr>
    </tbody>
  </table>

</div>


<div align="center">
  <figure>
    <div>
      <img width="600" height="361" alt="image" src="https://github.com/user-attachments/assets/572cb2b4-810a-45fd-89b4-590e0ef42f88" />
    </div>
    <figcaption><i>Figure 5: Scalability Comparison in Shared Memory Version</i></figcaption>
  </figure>
</div>
<br/>

The execution time decreases as thread count increases. OpenMP with 8 threads achieves the best time (0.0164s) compared to serial 
(0.0587s). Performance improves up to 8 threads, but using 12 threads shows slight overhead.

- For Distributed Memory

<div align="center">

  <table>
    <caption><strong>Table 4: Scalability Comparison in Distributed Memory Version</strong></caption>
    <thead>
      <tr>
        <th>Version</th>
        <th>Execution Time (sec)</th>
      </tr>
    </thead>
    <tbody>
      <tr><td>Serial</td><td>0.0587</td></tr>
      <tr><td>MPI - 2 processes</td><td>0.1210</td></tr>
      <tr><td>MPI - 4 processes</td><td>0.1873</td></tr>
      <tr><td>MPI - 8 processes</td><td>0.3098</td></tr>
    </tbody>
  </table>

</div>

<div align="center">
  <figure>
    <div>
      <img width="601" height="361" alt="image" src="https://github.com/user-attachments/assets/387723f3-8be5-4847-b714-c7a530aa7302" />
    </div>
    <figcaption><i>Figure 6: Scalability Comparison in Distributed Memory Version</i></figcaption>
  </figure>
</div>
<br/>

In the distributed memory model using MPI, execution time increases as the number of processes grows. With 2 processes, 
the time is 0.1210 seconds slower than the serial version (0.0587s). As the number of processes increases to 4 and 8, 
execution time rises to 0.1873s and 0.3098s, respectively, indicating significant communication overhead that outweighs 
the benefits of parallelism in this scenario.

- For Hybrid Version

<div align="center">
  <table>
    <caption><strong>Table 5: Scalability Comparison in Hybrid Version</strong></caption>
    <thead>
      <tr>
        <th>Version</th>
        <th>Execution Time (sec)</th>
      </tr>
    </thead>
    <tbody>
      <tr><td>Serial</td><td>0.0587</td></tr>
      <tr><td>OpenMP+MPI 2p × 4t</td><td>0.1073</td></tr>
      <tr><td>OpenMP+MPI 2p × 8t</td><td>0.1179</td></tr>
      <tr><td>OpenMP+MPI 2p × 12t</td><td>0.1377</td></tr>
      <tr><td>OpenMP+MPI 4p × 4t</td><td>0.2215</td></tr>
      <tr><td>OpenMP+MPI 4p × 8t</td><td>0.1633</td></tr>
      <tr><td>OpenMP+MPI 4p × 12t</td><td>0.1707</td></tr>
    </tbody>
  </table>

</div>

<div align="center">
  <figure>
    <div>
      <img width="601" height="361" alt="image" src="https://github.com/user-attachments/assets/cc89aaf5-adbf-4cfb-99ea-9bf3cf5c486f" />
    </div>
    <figcaption><i>Figure 7: Scalability Comparison in Hybrid Version</i></figcaption>
  </figure>
</div>
<br/>

In the hybrid model combining OpenMP and MPI, performance shows a mixed trend. While parallelism through OpenMP improves computation 
within each process, the MPI overhead still affects overall time. The best result among hybrid cases is with 2 processes and 4 
threads (0.1073s), which is better than pure MPI but still slower than OpenMP alone. As the number of processes and threads 
increases, the execution time tends to grow due to coordination and communication costs.
