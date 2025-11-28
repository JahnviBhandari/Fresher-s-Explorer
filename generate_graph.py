import csv
import random
import math

def calculate_weight(place1, place2):
    """Calculate weight based on distance difference and random factors"""
    dist_diff = abs(place1['distance'] - place2['distance'])
    weight = dist_diff * 0.8 + random.uniform(1, 5)
    return round(weight, 2)

def generate_graph_from_csv(csv_file, graph_file, max_vertices=50, max_edges_per_vertex=3):
    """Generate graph data from CSV places"""

    places = []
    with open(csv_file, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            places.append({
                'name': row['Name'],
                'type': row['Type'],
                'distance': float(row['Distance_km']),
                'rating': float(row['Rating'])
            })
    
    if len(places) > max_vertices:
        # Prioritize higher rated places
        places.sort(key=lambda x: x['rating'], reverse=True)
        places = places[:max_vertices]
    
    print(f"Generating graph with {len(places)} vertices...")
    
    with open(graph_file, 'w', encoding='utf-8') as f:
        # Write vertices
        for place in places:
            clean_name = place['name'].replace(' ', '_').replace(',', '')
            f.write(f"VERTEX {clean_name}\n")
        
        f.write("\n")
        
        edges_generated = 0
        for i, place1 in enumerate(places):
            connections = 0
            place1_clean = place1['name'].replace(' ', '_').replace(',', '')
   
            for j, place2 in enumerate(places):
                if i != j and connections < max_edges_per_vertex:
                    if (place1['type'] == place2['type'] or 
                        random.random() < 0.3): 
                        place2_clean = place2['name'].replace(' ', '_').replace(',', '')
                        weight = calculate_weight(place1, place2)
                        
                        f.write(f"EDGE {place1_clean} {place2_clean} {weight}\n")
                        connections += 1
                        edges_generated += 1
        
        print(f"Generated {edges_generated} edges")

def generate_smart_graph(csv_file, graph_file, max_vertices=80):
    """Generate a more realistic graph based on place types and distances"""
    
    places = []
    with open(csv_file, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            places.append({
                'name': row['Name'],
                'type': row['Type'],
                'distance': float(row['Distance_km']),
                'rating': float(row['Rating'])
            })

    places_by_type = {}
    for place in places:
        ptype = place['type']
        if ptype not in places_by_type:
            places_by_type[ptype] = []
        places_by_type[ptype].append(place)

    selected_places = []
    for ptype, type_places in places_by_type.items():
        type_places.sort(key=lambda x: x['rating'], reverse=True)
        selected_places.extend(type_places[:min(8, len(type_places))])

    all_sorted = sorted(places, key=lambda x: x['rating'], reverse=True)
    selected_places.extend(all_sorted[:20])

    unique_places = {}
    for place in selected_places:
        unique_places[place['name']] = place
    selected_places = list(unique_places.values())[:max_vertices]
    
    print(f"Selected {len(selected_places)} places for graph generation")
    
    with open(graph_file, 'w', encoding='utf-8') as f:
        for place in selected_places:
            clean_name = place['name'].replace(' ', '_').replace(',', '')
            f.write(f"VERTEX {clean_name}\n")
        
        f.write("\n")
        edges_generated = 0
        for ptype, type_places in places_by_type.items():
            type_in_selected = [p for p in selected_places if p['type'] == ptype]
            for i in range(len(type_in_selected)):
                for j in range(i+1, min(i+4, len(type_in_selected))):  # Connect to next 3 places
                    p1 = type_in_selected[i]
                    p2 = type_in_selected[j]
                    weight = calculate_weight(p1, p2)
                    p1_clean = p1['name'].replace(' ', '_').replace(',', '')
                    p2_clean = p2['name'].replace(' ', '_').replace(',', '')
                    f.write(f"EDGE {p1_clean} {p2_clean} {weight}\n")
                    edges_generated += 1

        high_rated = sorted(selected_places, key=lambda x: x['rating'], reverse=True)[:15]
        for i in range(len(high_rated)):
            for j in range(i+1, min(i+3, len(high_rated))):
                p1 = high_rated[i]
                p2 = high_rated[j]
                weight = calculate_weight(p1, p2)
                p1_clean = p1['name'].replace(' ', '_').replace(',', '')
                p2_clean = p2['name'].replace(' ', '_').replace(',', '')
                f.write(f"EDGE {p1_clean} {p2_clean} {weight}\n")
                edges_generated += 1
        for i in range(min(50, len(selected_places))):
            p1 = random.choice(selected_places)
            p2 = random.choice(selected_places)
            if p1 != p2 and random.random() < 0.2:  # 20% chance
                weight = calculate_weight(p1, p2)
                p1_clean = p1['name'].replace(' ', '_').replace(',', '')
                p2_clean = p2['name'].replace(' ', '_').replace(',', '')
                f.write(f"EDGE {p1_clean} {p2_clean} {weight}\n")
                edges_generated += 1
        
        print(f"Generated {edges_generated} total edges")
if __name__ == "__main__":
    csv_filename = "dehradun_data.csv"
    graph_filename = "graph.txt"
    
    print("Generating graph file from CSV data")
    generate_smart_graph(csv_filename, graph_filename, max_vertices=60)
    print(f"Graph data saved to {graph_filename}")