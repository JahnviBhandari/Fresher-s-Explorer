import requests
import csv
import time
import math

API_KEY = "AIzaSyAbptfQ5I0tzRLHbXk5cjkzV7u5JECtSpk"  
BASE_URL = "https://maps.googleapis.com/maps/api/place/textsearch/json"
CSV_FILE_NAME = "dehradun_data.csv"
LOCATION_QUERY = "Dehradun"

SEARCH_TYPES = ["cafe", "tourist attraction", "museum", "park", "restaurant", "hotel", "landmark"]

REF_LAT, REF_LON = 30.2724, 78.0631

all_spots = []
collected_place_ids = set() 
def calculate_distance_km(lat1, lon1, lat2, lon2):
    R = 6371  # Earth radius in kilometers
    lat1_rad, lon1_rad = math.radians(lat1), math.radians(lon1)
    lat2_rad, lon2_rad = math.radians(lat2), math.radians(lon2)
    dlat = lat2_rad - lat1_rad
    dlon = lon2_rad - lon1_rad
    a = math.sin(dlat / 2)**2 + math.cos(lat1_rad) * math.cos(lat2_rad) * math.sin(dlon / 2)**2
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
    distance = R * c
    return round(distance, 2)

def fetch_places(query_text):
    global all_spots
    params = {
        "query": f"{query_text} in {LOCATION_QUERY}",
        "key": API_KEY
    }   
    next_page_token = None   
    while True:
        if next_page_token:
            params['pagetoken'] = next_page_token
            time.sleep(2)  # Required delay for pagetoken
        
        print(f"Searching for: {query_text}")
        response = requests.get(BASE_URL, params=params)
        data = response.json()
        
        if data.get("status") not in ["OK", "ZERO_RESULTS"]:
            print(f"API Error for {query_text}: {data.get('status')}. Message: {data.get('error_message')}")
            break

        for place in data.get("results", []):
            place_id = place.get("place_id")
            if place_id and place_id not in collected_place_ids:
                collected_place_ids.add(place_id)
                
                spot_lat = place["geometry"]["location"]["lat"]
                spot_lon = place["geometry"]["location"]["lng"]
                
                spot = {
                    "name": place.get("name", "N/A").replace(',', ''),
                    "type": query_text, 
                    "rating": place.get("rating", 0.0),
                    "distance_km": calculate_distance_km(REF_LAT, REF_LON, spot_lat, spot_lon)
                }
                all_spots.append(spot)

        next_page_token = data.get("next_page_token")
        if not next_page_token:
            break
if __name__ == "__main__":
    print(f"Using reference point near Graphic Era University: ({REF_LAT}, {REF_LON})\n")

    for place_type in SEARCH_TYPES:
        fetch_places(place_type)
        
    print(f"\nTotal Unique Spots Collected: {len(all_spots)}")
    
    # Write data to CSV
    with open(CSV_FILE_NAME, 'w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerow(["Name", "Type", "Rating", "Distance_km"])  # Header row
        
        for spot in all_spots:
            writer.writerow([
                spot["name"],
                spot["type"],
                spot["rating"],
                spot["distance_km"]
            ])

    print(f"\nData saved to {CSV_FILE_NAME}. You can now run your C program.")

    
