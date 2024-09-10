#!/usr/bin/env python3

import urllib.request, json
import json
import pprint
import sys
import os


absPath = os.path.realpath(__file__)[:-len("script.py")]

data    = {}
totStr  = ""



static_format = "https://api.koda.trafiklab.se/KoDa/api/v2/gtfs-static/{operator}?date={date}&key={api_key}"

realtime_format = "https://api.koda.trafiklab.se/KoDa/api/v2/gtfs-rt/{operator}/{feed}?date={date}&key={api_key}"

arg_operator = "sl"

arg_api_key_realtime= "db18518e9d4749d3afe8efbb51ee9f16"
arg_api_key_static  = "20c6ab87676f4e0baec3769749de3ab7"

arg_feed = "ServiceAlerts"

arg_date = "2024-09-09"


# "https://api.koda.trafiklab.se/KoDa/api/v2/gtfs-static/sl?date=2024-09-09&key=48128affcce945879ee1c5fc7e30e57f"


print("\nprinted: \"", static_format.format(operator = arg_operator, date = arg_date, api_key = arg_api_key_realtime), "\"\n", sep="")

with urllib.request.urlopen(static_format.format(operator = arg_operator, date = arg_date, api_key = arg_api_key_realtime)) as url:
    data = json.load(url)

