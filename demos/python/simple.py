import random

from gi.repository import Gtk, Gdk, Uber

def random_value(linegraph, linenum, userdata):
    return random.random()*100

w = Uber.Window()
g = Uber.LineGraph()

g.props.autoscale = True
#g.set_dps(50)
l = Uber.Label(text="Random")
c = Gdk.color_parse("#729fcf")
g.add_line(c,l)
g.set_data_func(random_value, None)

w.add_graph(g, "Test")
w.show_all()
w.connect("delete-event", Gtk.main_quit)

Gtk.main()
