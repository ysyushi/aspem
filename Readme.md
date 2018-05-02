# AspEm

This repository provides codes and data for the paper:<br>
> AspEm: Embedding Learning by Aspects in Heterogeneous Information Networks<br>
> Yu Shi, Huan Gui, Qi Zhu, Lance Kaplan, and Jiawei Han.<br>
> In Proceedings of the 2018 SIAM International Conference on Data Mining, SIAM, 2018.<br>

Particularly, it includes (1) a reference implementation of incompatibility measure, (2) ad-hoc implementations of the single-aspect embedding algorithm for the datasets used in the paper, (3) the IMDb dataset (the full DBLP dataset is excluded from this repository due to its file size), and (4) the class labels used in the DBLP classification tasks.

### Basic Usage

#### Input

1. The supported input HIN file should contain all edges of the input HIN. Each line corresponds to an edge, with the format

		node_1 node_2 edge_weight edge_type
					
	Note that node_1 and node_2 should be in the form
			
		node_type:node_name

	An example input HIN file can be found at ``data/imdb/imdb.hin``.

2. Additionally, to run the ad-hoc implementation of the single-aspect embedding algorithm for star-schema datasets, one should also have a file of all center nodes (e.g., ``data/imdb/movie.node``) and a file of all attribute nodes as input (e.g., ``data/imdb/uadg.node``).


#### Execute

To measure the incompactibility of all base aspects in an HIN:<br/>

	``$ python src/calc_base_aspect_inconsistency.py --input $input-hin-file --output $base-aspect-inc-file [optional: --sample-rate $sample-rate] ``

To aggregate incompactibility for all base aspects from the result of the previous step:<br/>

	``$ python src/agg_aspect_inconsistency.py $base-aspect-inc-file ``

As an exmaple, to calculate the incompatibility of each aspect of the IMDb dataset, execute the following commands sequentially:<br/>

	``$ python src/calc_base_aspect_inconsistency.py --input data/imdb/imdb.hin --output data/imdb/imdb_base_aspect_inc.csv ``
	``$ python src/agg_aspect_inconsistency.py $data/imdb/imdb_base_aspect_inc.csv ``

To execute the ad-hoc implementation of the embedding algorithm, one should makefile in the corresponding source code directory in ``src/``, and then execute the binary code in its ``bin/``.

As an example, to embed the IMDb network with only attribute node types ``user`` and ``director``, execute the following commands sequentially:

	``$ cd src/emb_imdb/; make; cd ../../ ``
	``$ ./src/emb_imdb/bin/emb_imdb  -types ud -hin data/imdb/imdb.hin -center data/imdb/movie.node -attribute data/imdb/uadg.node -output data/imdb/attribute.emb -output-center data/imdb/center.emb ``

### Class Labels for DBLP Classification

In the DBLP experiment of the paper, two classification tasks were conducted based on the two class label files in 
	
	data/class_label/

### Citing
If you find *PReP* useful for your research, please consider citing the following paper:

	@inproceedings{shi2018aspem,
	author = {Shi, Yu and Gui, Huan and Zhu, Qi and Kaplan, Lance and Han, Jiawei},
	 title = {AspEm: Embedding Learning by Aspects in Heterogeneous Information Networks},
	 booktitle = {Proceedings of the 2018 SIAM International Conference on Data Mining},
	 year = {2018},
	 organization={SIAM}
	}


### Miscellaneous

Please send any questions you might have about the codes and/or the algorithm to <yushi2@illinois.edu>.

*Note:* This is only a reference implementation of the *AspEm* algorithm. As discussed in the paper, AspEm is a flexible framework and one can choose their favorite network embedding algorithm to embed every single aspect.