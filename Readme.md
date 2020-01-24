This folder contains the code used for a number of experiments and simulations, as published in <link to appear>.

## Usage

The codes should compile with the makefiles within each folder, once they have been edited to point to the installation directory for Gurobi 7.5.2.

Once compiled into a binary executable, the codes can each be run as follow (replace BINARY with the name of the binary)

  ./BINARY /path/to/input/files/ input-file.txt logfile.txt

where the input is given by /path/to/input/files/input-file.txt, and logfile.txt is a (possibly not yet existent) log file. The exact input file format is described below.

## Models

The following tables show the various models implemented by these codes. For details on the M-values, see the paper as linked above.

| Folder name         | Model                       |
|---------------------|-----------------------------|
| 1\_MM\_PURE         | M1                          |
| 2\_MM\_VI           | M2                          |
| 3\_MM\_PREVI        | M3                          |
| 4\_MM\_PREVICM      | M4                          |
| 5\_MM\_PREVICMBV1   | M5                          |
| 6\_MM\_PREVICMBV2   | M7                          |
| 7\_MM\_PREVICMBV3   | M9                          |
| 8\_MM\_PREVICMBV1P  | M6                          |
| 9\_MM\_PREVICMBV2P  | M8                          |
| A\_MM\_PREVICMBV3P  | M10                         |
| B\_MM\_PREVICMBV23  | M11                         |
| C\_MM\_PREVIBV23    | M12                         |
| D\_MM\_PREVICMBV123 | M13                         |
| E\_MM               | M13 as above                |
| F\_BIS              | M13 but with BIS stability  |
| G\_KPR              | M13 but with KPR stability  |
| H\_PLUS             | M13 but with KPR+ stability |

## File format

Each file contains one instance, and both HRT and HRTC instances use the same format. The first line in each file denotes s, the number of single doctors in the instance. The next line denotes c, the number of couples in the instance, and the third line marks h, the number of hospitals in the instance.

Following this, we have s+c lines with the doctors preferences. The first s lines are for each single doctor, where each line begins first with an identifier for the doctor (a number between 1 and s inclusive), and then a space separated list of the preferences of the doctor in descending order of preference.
The next c lines give the preferences for the couples. Note that for couples, each line begins with two identifiers (one for each doctor) and then is followed by some number of pairs of integers. These preferences should be read two at a time. For instance, the line

  17 19 3 3 2 2

indicates a couple whose doctors are identified as 17 and 19. The top preference of this couple is for both members to be assigned to hospital 3, and the second preference of this couple is for both members to be assigned to hospital 2.

Following the preferences for couples, we have h lines marking the hospitals' preferences. These begin with an identifier, which is followed by the capacity of the hospital, and then the preferences of that hospital over individual doctors. A hospitals' preference list may include ties, which are marked by brackets, and note that a hospital does not have preferences for the couple as a pair, but for the individual members of the couple.
