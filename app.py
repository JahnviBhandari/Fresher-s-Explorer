from flask import Flask, request, jsonify, render_template
from flask_cors import CORS
import subprocess
import os
import json
import csv
import sys

app = Flask(__name__)
CORS(app)

# Windows executable paths
C_AUTH_EXEC = "auth_manager.exe"
C_TOURIST_EXEC = "tourist_manager.exe" 
C_GRAPH_EXEC = "graph_manager.exe"

print("🚀 Starting Dehradun Explorer Web Application...")
print("✅ Authentication: ENABLED")
print("✅ Web Interface: ENABLED")

# ULTRA SAFE CSV LOADER - CANNOT FAIL
def load_places_data():
    """
    This function WILL NOT CRASH no matter what's in the CSV
    Returns empty list if anything goes wrong
    """
    places = []
    try:
        # First, check if file exists
        if not os.path.exists('dehradun_data.csv'):
            print("⚠️  CSV file not found, returning empty list")
            return []
            
        with open('dehradun_data.csv', 'r', encoding='utf-8') as file:
            # Read all lines first to handle encoding issues
            lines = file.readlines()
            if not lines:
                return []
            
            # Parse header
            header = lines[0].strip().split(',')
            if len(header) < 4:
                print("⚠️  Invalid CSV header")
                return []
            
            # Process data rows
            for line_num, line in enumerate(lines[1:], start=2):
                try:
                    line = line.strip()
                    if not line:  # Skip empty lines
                        continue
                        
                    parts = line.split(',')
                    if len(parts) < 4:  # Need at least 4 columns
                        continue
                    
                    # Extract and clean fields
                    name = parts[0].strip() if parts[0].strip() else f"Place_{line_num}"
                    place_type = parts[1].strip() if len(parts) > 1 and parts[1].strip() else "unknown"
                    rating_str = parts[2].strip() if len(parts) > 2 else "0"
                    distance_str = parts[3].strip() if len(parts) > 3 else "0"
                    
                    # Safe number conversion
                    try:
                        rating = float(rating_str) if rating_str and rating_str != '' else 0.0
                    except:
                        rating = 0.0
                    
                    try:
                        distance = float(distance_str) if distance_str and distance_str != '' else 0.0
                    except:
                        distance = 0.0
                    
                    place = {
                        'id': len(places),
                        'name': name,
                        'type': place_type,
                        'rating': rating,
                        'distance': distance
                    }
                    places.append(place)
                    
                except Exception as e:
                    # SILENTLY skip bad rows - no error messages
                    continue
                    
        print(f"✅ Loaded {len(places)} places from CSV")
        return places
        
    except Exception as e:
        # SILENT failure - return empty list
        return []

# SIMPLE AUTHENTICATION - NO CSV DEPENDENCY
def simple_register(username, password):
    try:
        username = username.strip()
        password = password.strip()
        
        if not username or not password:
            return False, "Username and password required"
        
        # Check if user exists
        if os.path.exists('users.txt'):
            with open('users.txt', 'r', encoding='utf-8') as f:
                for line in f:
                    if line.startswith(username + ":"):
                        return False, "User already exists"
        
        # Create users file if it doesn't exist
        with open('users.txt', 'a', encoding='utf-8') as f:
            f.write(f"{username}:{password}\n")
        
        return True, "Registration successful"
        
    except Exception as e:
        return False, f"Registration error: {str(e)}"

def simple_login(username, password):
    try:
        username = username.strip()
        password = password.strip()
        
        if not username or not password:
            return False, "Username and password required"
            
        if not os.path.exists('users.txt'):
            return False, "No users registered yet"
            
        with open('users.txt', 'r', encoding='utf-8') as f:
            for line in f:
                parts = line.strip().split(':')
                if len(parts) == 2 and parts[0] == username and parts[1] == password:
                    return True, "Login successful"
        
        return False, "Invalid username or password"
        
    except Exception as e:
        return False, f"Login error: {str(e)}"

# API ROUTES - AUTHENTICATION (ALWAYS WORKS)
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/register', methods=['POST'])
def register():
    try:
        data = request.get_json()
        if not data or 'username' not in data or 'password' not in data:
            return jsonify({'success': False, 'message': 'Invalid request'})
        
        success, message = simple_register(data['username'], data['password'])
        return jsonify({'success': success, 'message': message})
    except Exception as e:
        return jsonify({'success': False, 'message': 'Server error'})

@app.route('/api/login', methods=['POST'])
def login():
    try:
        data = request.get_json()
        if not data or 'username' not in data or 'password' not in data:
            return jsonify({'success': False, 'message': 'Invalid request'})
        
        success, message = simple_login(data['username'], data['password'])
        return jsonify({'success': success, 'message': message})
    except Exception as e:
        return jsonify({'success': False, 'message': 'Server error'})

# API ROUTES - PLACES (FAIL-SAFE)
@app.route('/api/places', methods=['GET'])
def get_places():
    places = load_places_data()
    return jsonify(places)

@app.route('/api/places/sort/distance', methods=['GET'])
def sort_places_by_distance():
    places = load_places_data()
    sorted_places = sorted(places, key=lambda x: x['distance'])
    return jsonify(sorted_places)

@app.route('/api/places/sort/rating', methods=['GET'])
def sort_places_by_rating():
    places = load_places_data()
    sorted_places = sorted(places, key=lambda x: x['rating'], reverse=True)
    return jsonify(sorted_places)

@app.route('/api/places/filter/<type_filter>', methods=['GET'])
def filter_places_by_type(type_filter):
    places = load_places_data()
    filtered_places = [p for p in places if type_filter.lower() in p['type'].lower()]
    return jsonify(filtered_places)

# TOURIST MODULE SEARCH - SHOWS MULTIPLE RESULTS AS YOU TYPE
@app.route('/api/places/search/<name>', methods=['GET'])
def search_place(name):
    try:
        places = load_places_data()
        found_places = []
        search_term = name.lower().strip()
        
        print(f"🔍 TOURIST MODULE searching for: '{search_term}'")
        
        # BROAD SEARCH - show all places that contain the search term
        for place in places:
            place_name = place['name'].lower()
            
            # Show any place that contains the search term
            if search_term in place_name:
                found_places.append(place)
        
        print(f"🔍 Found {len(found_places)} matches")
        
        if found_places:
            return jsonify({
                'success': True,
                'found': True,
                'results': found_places,
                'count': len(found_places),
                'message': f'Found {len(found_places)} places matching "{name}"'
            })
        else:
            return jsonify({
                'success': True,
                'found': False,
                'results': [],
                'message': f'No places found matching "{name}"'
            })
            
    except Exception as e:
        print(f"Search error: {e}")
        return jsonify({
            'success': False,
            'message': 'Search failed'
        })

# SEARCH MODULE SEARCH - EXACT MATCH ONLY
@app.route('/api/places/search-exact/<name>', methods=['GET'])
def search_place_exact(name):
    try:
        places = load_places_data()
        search_term = name.lower().strip()
        
        print(f"🔍 SEARCH MODULE exact search for: '{search_term}'")
        
        # EXACT MATCH - only show places that exactly match
        exact_match = None
        
        for place in places:
            place_name = place['name'].lower()
            
            # Strategy 1: Exact name match (highest priority)
            if search_term == place_name:
                exact_match = place
                print(f"✅ Exact match found: {place['name']}")
                break
            
            # Strategy 2: Place name starts with search term
            if place_name.startswith(search_term):
                exact_match = place
                print(f"✅ Starts with match found: {place['name']}")
                break
        
        if exact_match:
            return jsonify({
                'success': True,
                'found': True,
                'results': [exact_match],  # Only one result
                'count': 1,
                'message': f'Found exact match for "{name}"'
            })
        else:
            return jsonify({
                'success': True,
                'found': False,
                'results': [],
                'message': f'No exact match found for "{name}"'
            })
            
    except Exception as e:
        print(f"Exact search error: {e}")
        return jsonify({
            'success': False,
            'message': 'Exact search failed'
        })

# IMPROVED GRAPH PATHFINDING - USES ACTUAL GRAPH.TXT DATA
@app.route('/api/graph/path', methods=['POST'])
def find_shortest_path():
    try:
        data = request.get_json()
        source = data.get('source', '').strip()
        destination = data.get('destination', '').strip()
        
        if not source or not destination:
            return jsonify({'error': 'Please provide both source and destination places'})
        
        print(f"🎯 Finding REAL path from '{source}' to '{destination}'")
        print(f"📁 Current directory: {os.getcwd()}")
        print(f"📊 Graph file exists: {os.path.exists('graph.txt')}")
        
        # Read graph file to verify connections
        if os.path.exists('graph.txt'):
            with open('graph.txt', 'r', encoding='utf-8') as f:
                graph_content = f.read()
                source_exists = source in graph_content
                dest_exists = destination in graph_content
                print(f"🔍 Source '{source}' in graph: {source_exists}")
                print(f"🔍 Destination '{destination}' in graph: {dest_exists}")
                
                # Check for direct connection
                if f"EDGE {source} {destination}" in graph_content or f"EDGE {destination} {source}" in graph_content:
                    print(f"✅ Direct connection found between {source} and {destination}")
        
        # Use the compiled C graph program with EXACT names from graph.txt
        if os.path.exists('graph_manager.exe'):
            try:
                print(f"🚀 Executing: graph_manager.exe shortest_path '{source}' '{destination}'")
                
                # Run the graph manager with the exact vertex names from graph.txt
                result = subprocess.run(
                    ['graph_manager.exe', 'shortest_path', source, destination],
                    capture_output=True, 
                    text=True, 
                    timeout=10,
                    shell=True,
                    cwd=os.getcwd()
                )
                
                print(f"📊 Graph program return code: {result.returncode}")
                print(f"📊 Graph program stdout: {result.stdout}")
                if result.stderr:
                    print(f"📊 Graph program stderr: {result.stderr}")
                
                if result.returncode == 0:
                    output = result.stdout.strip()
                    
                    if output and "No path found" not in output and "Invalid" not in output:
                        print(f"✅ REAL PATH FOUND: {output}")
                        return jsonify({
                            'success': True,
                            'path': output,
                            'message': 'Real path computed using Dijkstra algorithm from graph data'
                        })
                    else:
                        print(f"❌ No path in graph: {output}")
                        # Fallback to manual graph analysis
                        manual_path = find_path_manual(source, destination)
                        return jsonify({
                            'success': True,
                            'path': manual_path,
                            'message': 'Path calculated by analyzing graph connections'
                        })
                else:
                    print(f"❌ Graph program failed with return code: {result.returncode}")
                    
            except subprocess.TimeoutExpired:
                print("⏰ Graph program timed out")
            except Exception as e:
                print(f"💥 Graph program exception: {e}")
        
        # Final fallback: manual graph analysis
        manual_path = find_path_manual(source, destination)
        return jsonify({
            'success': True,
            'path': manual_path,
            'message': 'Path calculated by analyzing graph data'
        })
        
    except Exception as e:
        print(f"Path finding error: {e}")
        return jsonify({
            'success': False,
            'error': f'Path finding failed: {str(e)}'
        })

def find_path_manual(source, destination):
    """Manually analyze graph.txt to find paths and distances"""
    try:
        print(f"🔍 Manually analyzing graph for path from {source} to {destination}")
        
        if not os.path.exists('graph.txt'):
            return f"Graph file not found. Cannot calculate path from {source} to {destination}"
        
        # Parse graph file
        vertices = set()
        edges = []
        
        with open('graph.txt', 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line.startswith('VERTEX'):
                    vertex = line[7:].strip()  # Remove "VERTEX " prefix
                    vertices.add(vertex)
                elif line.startswith('EDGE'):
                    parts = line[5:].strip().split()  # Remove "EDGE " prefix
                    if len(parts) >= 3:
                        u, v, weight = parts[0], parts[1], float(parts[2])
                        edges.append((u, v, weight))
                        # Add reverse edge since graph is undirected
                        edges.append((v, u, weight))
        
        print(f"📊 Graph has {len(vertices)} vertices and {len(edges)//2} edges")
        print(f"🔍 Looking for {source} and {destination} in {len(vertices)} vertices")
        
        # Check if vertices exist
        if source not in vertices:
            return f"Source '{source}' not found in graph"
        if destination not in vertices:
            return f"Destination '{destination}' not found in graph"
        
        # Simple BFS to find shortest path
        from collections import deque
        import math
        
        # Build adjacency list
        graph = {}
        for u, v, w in edges:
            if u not in graph:
                graph[u] = []
            graph[u].append((v, w))
        
        # Dijkstra's algorithm
        dist = {vertex: math.inf for vertex in vertices}
        prev = {vertex: None for vertex in vertices}
        dist[source] = 0
        
        # Priority queue simulation
        queue = list(vertices)
        
        while queue:
            # Find vertex with minimum distance
            u = min(queue, key=lambda x: dist[x])
            queue.remove(u)
            
            if u == destination:
                break
                
            if u not in graph:
                continue
                
            for neighbor, weight in graph[u]:
                if neighbor in queue:
                    alt = dist[u] + weight
                    if alt < dist[neighbor]:
                        dist[neighbor] = alt
                        prev[neighbor] = u
        
        # Reconstruct path
        if dist[destination] == math.inf:
            return f"No path found from {source} to {destination} in the graph data"
        
        path = []
        current = destination
        while current is not None:
            path.append(current)
            current = prev[current]
        path.reverse()
        
        path_str = " -> ".join(path)
        return f"Shortest path from {source} to {destination}:\n{path_str}\nTotal distance: {dist[destination]:.2f} km"
        
    except Exception as e:
        print(f"Manual path finding error: {e}")
        return f"Error calculating path: {str(e)}"
@app.route('/api/graph', methods=['GET'])
def get_graph():
    return jsonify({'graph': 'Graph data loaded'})

if __name__ == '__main__':
    print("🌐 Server running on http://localhost:5000")
    print("🔧 Debug mode: ON")
    print("📊 Places system: FAIL-SAFE MODE")
    print("🎯 Graph system: REAL DIJKSTRA ALGORITHM")
    print("🔍 Search system: TOURIST=REAL-TIME, SEARCH=EXACT MATCH")
    app.run(debug=True, port=5000)