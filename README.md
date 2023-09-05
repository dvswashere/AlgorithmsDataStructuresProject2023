# Highway pathfinder algorithm

This repository contains my solution to the 2023 Algorithms and data structures final project for the BSc in Engineering of Computing Systems in
Politecnico di Milano.

Given an input in stdin, the program, which must be written in C, has to output the solution to stdout.

## Project specifications

The highway is composed of service stations, each identified by its distance (in kms) from the start of the highway.  Every station possesses a
collection of at most 512 electric vehicles.  Every electric vehicle has got an autonomy in kms, which is a positive integer.

The goal is to find the path with the least amount of stops from a starting station to the destination, which are arbitrarily chosen from the set
of stations in the highway.  At each stop, it's necessary to change vehicle and start using a new one from the options available in that particular
station.  Beware that the algorithms only plans the shortest path, thus, the vehicles don't move from a station to another while calculating the
solution.

### Input commands and expected output

The following table displays the format of the commands which can be inputted to the algorithm and the expected output format.

| Command | Effect | Expected output |
|:--------|:-------|:----------------|
| aggiungi-stazione distance number-cars car-1 ... car-n | Adds a station to the highway at the specified distance from its start.  Also adds number-cars cars to the station.  The cars' autonomies are listed right after the number of vehicles in the station.  If a station at the given distance already exists, the addition doesn't take place. | aggiunta / non aggiunta |
| demolisci-stazione distance | Removes the service station located in the highway at the specified distance if it exists. | demolita / non demolita |
| aggiungi-auto station-distance car-autonomy | Adds a car with the specified autonomy to the station at the given distance, if the station exists.  There can be multiple cars with same autonomy in the same station. | aggiunta / non aggiunta |
| rottama-auto station-distance car-autonomy | Removes the car with specified autonomy from the station at the given distance, if both the station and that car exist. | rottamata / non rottamata |
| pianifica-percorso starting-station ending-station | Plans the path with the least stops between the two given stations, if at least a path exists.  The highway has two directions of travel, however, once a direction has been chosen, the other direction can't be used. | If the path exists: the sequence of stations used in the path in the order of travel, including the starting and ending stations.  Stations are printed as integers, separated by one space each, in a single line.  If the path doesn't exist: nessun percorso. |

### Multiple shortest paths

Between two stations there can be multiple path this minimal length, where length is intended as number of stops.  The algorithm must choose the 
path that in its ending part always contains the stations closest to the starting point of the highway.

For instance
```
km 0 ---- km 10 ---- km 20 ---- km 30 ---- km 40 ---- km 50 ---- km 60
          car 20     car 30     car 10     car 20     car 10
```
The correct path between 10 and 60 is:  10 20 40 60.  10 30 40 60 is incorrect, because 30 > 20.  Also 10 20 50 60 is incorrect, since 50 > 40.
