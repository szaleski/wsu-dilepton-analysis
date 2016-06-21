#### makeGIF_sym.py
This script will run the endpoint study, creating outputs of the chi^2 vs bias, KS test statistic vs bias, and comparisons of the curvature distributions for each bias value
```
Usage: makeGIF_sym.py -i inputfile.root --p100infile p100inputfile.root --p500infile p500inputfile.root -o outputfile.root [-d]

Options:
  -h, --help            show this help message and exit
  -i infile, --infile=infile
                        [REQUIRED] Location of the input ROOT files
  --p100infile=p100infile
                        [REQUIRED] Location of the comparison input p100 ROOT
                        file
  --p500infile=p500infile
                        [REQUIRED] Location of the comparison input p500 ROOT
                        file
  -b rebins, --rebins=rebins
                        [OPTIONAL] Number of bins to combine in the q/pT plot
                        (default is 1)
  -n biasbins, --biasbins=biasbins
                        [OPTIONAL] Total number of injected bias points
                        (default is 1000)
  -t totalbins, --totalbins=totalbins
                        [OPTIONAL] Total number of bins in the original
                        curvature distribution (default is 5000)
  -m maxbias, --maxbias=maxbias
                        [OPTIONAL] Maximum injected bias (default is 0.1
                        c/TeV)
  -s stepsize, --stepsize=stepsize
                        [OPTIONAL] Step size in the GIF (default is 1)
  -d, --debug           [OPTIONAL] Debug mode
  --histbase=histbase   [OPTIONAL] Base name of the histogram object (default
                        is "looseMuLower")
  --study=study         [OPTIONAL]  Study to perform (default is "pm", options
                        are: "pm","dmc[a-h]")
  --minpt=minpt         [OPTIONAL] Minimum pT cut to apply in the curvature
                        plots (default is 200 c/TeV)
  --residuals           [OPTIONAL] Display residuals
  --etaphi=etaphi       [OPTIONAL] Eta/Phi bin to use
  --mcclosure           [OPTIONAL] Do an MC closure test (requires 'mcbias')
  --mcbias=mcbias       [OPTIONAL] Value of injected bias when doing an MC
                        closure test (default is -0.2 c/TeV
Example usage:
  ./makeGIF_sym.py -i /afs/cern.ch/work/s/szaleski/public/forJared/CRAFT15_May/CosmicHistOut_TuneP.root --p100infile /afs/cern.ch/work/s/szaleski/public/forJared/StartupP100_May/CosmicHistOut_TuneP.root --p500infile /afs/cern.ch/work/s/szaleski/public/forJared/StartupP500_May/CosmicHistOut_TuneP.root -b 80 -n 200 --minpt 150 -t 1600 -s10 -m 0.8 --residuals
  ./makeGIF_sym.py -i /afs/cern.ch/work/s/szaleski/public/forJared/CRAFT15_May/CosmicHistOut_TuneP.root --p100infile /afs/cern.ch/work/s/szaleski/public/forJared/StartupP100_May/CosmicHistOut_TuneP.root --p500infile /afs/cern.ch/work/s/szaleski/public/forJared/StartupP500_May/CosmicHistOut_TuneP.root -b 80 -n 200 --minpt 150 -t 1600 -s10 -m 0.8 --residuals --mcclosure --mcbias -0.1
```

#### bsubEfficiency.py
This script will submit jobs for efficiency studies
```
Usage: bsubEfficiency.py [options]

Options:
  -h, --help            show this help message and exit
  -g, --gridproxy       [OPTIONAL] Generate a GRID proxy
  -i infiles, --infiles=infiles
                        [REQUIRED] Text file with list of input files to
                        process
  -t title, --title=title
                        [REQUIRED] Task title
  -x tool, --tool=tool  [REQUIRED] Tool name (default is efficiencyStudy)
  -d, --debug           [OPTIONAL] Run in debug mode, i.e., don't submit jobs,
                        just create them

```

#### bsubGenScaling.py
This script will perform some scaling tests
```
Usage: bsubGenScaling.py [options]

Options:
  -h, --help            show this help message and exit
  -g, --gridproxy       [OPTIONAL] Generate a GRID proxy
  -i infiles, --infiles=infiles
                        [REQUIRED] Text file with list of input files to
                        process
  -t title, --title=title
                        [REQUIRED] Task title
  -x tool, --tool=tool  [REQUIRED] Tool name (default is runGenL1Studies)
  -m, --mc              [OPTIONAL] Running ananalysis on MC
  -d, --debug           [OPTIONAL] Run in debug mode, i.e., don't submit jobs,
                        just create them

```

#### bsubSubmit.py
This script will help you submit jobs to the lxbatch (or any other bsub system)
It depends on ```macrotools.py```
```
Usage: bsubSubmit.py [options]

Options:
  -h, --help            show this help message and exit
  -n njobs, --njobs=njobs
                        [OPTIONAL] Number of jobs to submit (default is 10)
  -g, --gridproxy       [OPTIONAL] Generate a GRID proxy
  -i infiles, --infiles=infiles
                        [REQUIRED] Text file with list of input files to
                        process
  -t title, --title=title
                        [REQUIRED] Task title
  -x tool, --tool=tool  [REQUIRED] Tool name (default is Plot)
  -d, --debug           [OPTIONAL] Run in debug mode, i.e., don't submit jobs,
                        just create them
  -m maxbias, --maxbias=maxbias
                        [OPTIONAL] Maximum bias in the curvature to inject in
                        units of c/GeV (default = 0.0008)
  -a, --asymmetric      [OPTIONAL] Specify whether to create asymmetric
                        (absolute value) curvature (default = False)
  -p minpt, --minpt=minpt
                        [OPTIONAL] Minimum pT cut to apply to the muons
                        (default = 50.)
  -b nbiasbins, --nbiasbins=nbiasbins
                        [OPTIONAL] Number of steps to vary the injected bias
                        by (default = 200)
  --trigger             [OPTIONAL] Apply or not the fake L1SingleMu selection
  --mc                  [OPTIONAL] Whether or not running on MC
  --simlow=simlow       [OPTIONAL] Minimum pT cut to apply to the sim tracks
                        (only for MC)
  --simhigh=simhigh     [OPTIONAL] Maximum pT cut to apply to sim trkcs (only
                        for MC)

```

#### runEndpoint.py
This script will run the endpoint study
```
Usage: runEndpoint.py [options]

Options:
  -h, --help            show this help message and exit
  -n nbiasbins, --nbiasbins=nbiasbins
                        [OPTIONAL] Number of steps used to bias the curvature
                        (default is 2500)
  -t totalbins, --totalbins=totalbins
                        [OPTIONAL] Total number of bins in the original
                        curvature distribution (default is 1500)
  -r rebins, --rebins=rebins
                        [OPTIONAL] Number of bins to merge into one in the
                        input histograms (default is 1, no merging)
  -f factor, --factor=factor
                        [OPTIONAL] Multiplicative factor on the curvature
                        (default is 1000)
  -b maxbias, --maxbias=maxbias
                        [] Maximum bias that was injected into the curvature
                        (default is 0.0005/GeV
  -i infiledir, --infiledir=infiledir
                        [REQUIRED] Location of the input ROOT files
  -o outfile, --outfile=outfile
                        [REQUIRED] Name of the output ROOT file
  -s, --symmetric       [OPTIONAL] Are the curevature plots symmetric about 0,
                        if not specified, asymmetric is assumed
  -d, --debug           [OPTIONAL] Run in debug mode
  --histbase=histbase   [OPTIONAL] Base name of the histogram object (default
                        is "looseMuUpper")
  --obs=obs             [OPTIONAL] Name of the observed histogram object
                        (default is "Minus", using the negatively charged muon
                        as the observed value)
  --ref=ref             [OPTIONAL] Name of the reference histogram object
                        (default is "Plus", using the positively charged muon
                        as the reference value)

```

#### quickHistogramPlotter.py
This script will create a set of canvasses with the basic distributions for quick consumption.
It enables one to quickly check that sanity checks have passed

```
Usage: quickHistogramPlotter.py -i inputfile.root -o outputfile.root [-d]

Options:
  -h, --help            show this help message and exit
  -i infile, --infile=infile
                        [REQUIRED] Location of the input ROOT files
  -o outfile, --outfile=outfile
                        [REQUIRED] Name of the output ROOT file
  -b rebins, --rebins=rebins
                        [OPTIONAL] Number of bins to combine in the q/pT plot
                        (default is 1)
  -d, --debug           [OPTIONAL] Debug mode

Example usage:
./quickHistogramPlotter.py -i ~szaleski/work/public/forJared/MCStartupp100/CosmicHistOut_TuneP.root -o startup100_shawn.root -b25 -d
```
