import ROOT as r

def makeNicePlot(hist,params,debug=False):
    """makeNicePlot: takes a histogram object and a set of parameters and applies the settings
    params: a map of parameters
    params keys:
        "color": color to make the line/marker
        "marker": marker to draw at each point
        "stats": options to pass to SetOptStat
        "coords": x[x1,x2] and y[y1,y2] coordinates of the stats box
    """
    
    if debug:
        print hist
        print params

        #color,marker,coords,stats,
    hist.SetLineWidth(2)
    hist.SetLineColor(params["color"])
    hist.SetMarkerStyle(params["marker"])
    hist.SetMarkerColor(params["color"])
    hist.SetMarkerStyle(params["marker"])
    hstat = hist.FindObject("stats")
    if debug:
        print hstat
    hstat.SetTextColor(params["color"])
    hstat.SetOptStat(params["stats"])
    if params["coords"]["x"][0] > -0.1:
        hstat.SetX1NDC(params["coords"]["x"][0])
    if params["coords"]["x"][1] > -0.1:
        hstat.SetX2NDC(params["coords"]["x"][1])
    if params["coords"]["y"][0] > -0.1:
        hstat.SetY1NDC(params["coords"]["y"][0])
    if params["coords"]["y"][1] > -0.1:
        hstat.SetY2NDC(params["coords"]["y"][1])
    return hist

