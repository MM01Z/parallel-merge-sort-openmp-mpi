@echo off
REM -------------------------------
REM Run all PDC project executables
REM Metrics are written in code
REM -------------------------------

REM Path to executable in Code::Blocks
set EXE=bin\Debug\PDC_Project.exe

echo ==============================
echo Generating input
%EXE% generate
echo ==============================

echo.

echo ==============================
echo Running Sequential
echo ==============================
%EXE% seq

echo. 

echo ==============================
echo Running OpenMP
echo ==============================
%EXE% omp 4

echo.

echo ==============================
echo Running MPI (change number of processes in mpiexec)
echo ==============================
mpiexec -n 4 %EXE% mpi

echo.
				
echo ==============================
echo All runs completed. Metrics saved in results.csv
pause
