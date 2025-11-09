#!/usr/bin/env python3
"""
Backend Python Flask pour Simulateur IPC Unix
Gère le serveur C et les requêtes clients
"""

from flask import Flask, jsonify, request, send_file
from flask_cors import CORS
import subprocess
import os
import signal
import time
import random

app = Flask(__name__)
CORS(app)

# Variables globales
server_process = None
server_pid = None

@app.route('/')
def index():
    """Servir l'interface HTML"""
    return send_file('index.html')

@app.route('/api/server/start', methods=['POST'])
def start_server():
    """Démarrer le serveur C"""
    global server_process, server_pid
    
    try:
        # Vérifier si le serveur existe
        if not os.path.exists('./serveur'):
            return jsonify({
                'success': False,
                'error': 'Exécutable serveur non trouvé. Compilez d\'abord avec: make'
            })
        
        # Démarrer le serveur en arrière-plan
        server_process = subprocess.Popen(
            ['./serveur'],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            preexec_fn=os.setsid  # Créer un nouveau groupe de processus
        )
        
        server_pid = server_process.pid
        time.sleep(0.5)  # Attendre que le serveur démarre
        
        return jsonify({
            'success': True,
            'pid': server_pid,
            'message': 'Serveur démarré avec succès'
        })
        
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        })

@app.route('/api/server/stop', methods=['POST'])
def stop_server():
    """Arrêter le serveur C"""
    global server_process, server_pid
    
    try:
        if server_process and server_pid:
            # Envoyer SIGTERM pour arrêt propre
            os.killpg(os.getpgid(server_pid), signal.SIGTERM)
            server_process.wait(timeout=2)
            
            server_process = None
            server_pid = None
            
            # Nettoyer les FIFOs
            try:
                os.unlink('/tmp/fifo1')
                os.unlink('/tmp/fifo2')
            except:
                pass
            
            return jsonify({
                'success': True,
                'message': 'Serveur arrêté avec succès'
            })
        else:
            return jsonify({
                'success': False,
                'error': 'Aucun serveur en cours d\'exécution'
            })
            
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        })

@app.route('/api/client/request', methods=['POST'])
def client_request():
    """Traiter une requête client"""
    global server_pid
    
    try:
        if not server_pid:
            return jsonify({
                'success': False,
                'error': 'Le serveur n\'est pas démarré'
            })
        
        data = request.json
        client_id = data.get('clientId')
        num_values = data.get('numValues')
        
        # Validation
        if not client_id or not num_values:
            return jsonify({
                'success': False,
                'error': 'clientId et numValues requis'
            })
        
        if num_values < 1 or num_values > 20:
            return jsonify({
                'success': False,
                'error': 'numValues doit être entre 1 et 20'
            })
        
        # Vérifier si le client_web existe
        if not os.path.exists('./client_web'):
            # Si client_web n'existe pas, simuler la communication
            return simulate_request(client_id, num_values)
        
        # Exécuter le client_web
        result = subprocess.run(
            ['./client_web', str(server_pid), str(num_values)],
            capture_output=True,
            text=True,
            timeout=5
        )
        
        if result.returncode != 0:
            return jsonify({
                'success': False,
                'error': 'Erreur lors de l\'exécution du client',
                'details': result.stderr
            })
        
        # Parser la sortie
        output = result.stdout
        values = []
        sum_val = 0
        avg_val = 0.0
        
        for line in output.split('\n'):
            if 'Valeurs reçues:' in line:
                values_str = line.split(':')[1].strip()
                values = [int(x) for x in values_str.split()]
            elif 'Somme =' in line:
                sum_val = int(line.split('=')[1].strip())
            elif 'Moyenne =' in line:
                avg_val = float(line.split('=')[1].strip())
        
        return jsonify({
            'success': True,
            'clientId': client_id,
            'values': values,
            'sum': sum_val,
            'avg': avg_val
        })
        
    except subprocess.TimeoutExpired:
        return jsonify({
            'success': False,
            'error': 'Timeout: le serveur ne répond pas'
        })
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        })

def simulate_request(client_id, num_values):
    """Simuler une requête si client_web n'existe pas"""
    # Générer des valeurs aléatoires
    values = [random.randint(1, 100) for _ in range(num_values)]
    sum_val = sum(values)
    avg_val = round(sum_val / len(values), 2)
    
    return jsonify({
        'success': True,
        'clientId': client_id,
        'values': values,
        'sum': sum_val,
        'avg': avg_val,
        'simulated': True
    })

@app.route('/api/server/status', methods=['GET'])
def server_status():
    """Obtenir le statut du serveur"""
    global server_pid, server_process
    
    is_running = False
    if server_process and server_pid:
        # Vérifier si le processus est toujours vivant
        try:
            os.kill(server_pid, 0)
            is_running = True
        except OSError:
            is_running = False
            server_process = None
            server_pid = None
    
    return jsonify({
        'running': is_running,
        'pid': server_pid if is_running else None
    })

if __name__ == '__main__':
    print("🚀 Serveur Flask démarré sur http://localhost:5000")
    print("📋 Assurez-vous d'avoir compilé les programmes C avec: make")
    print("🌐 Ouvrez votre navigateur sur http://localhost:5000")
    print("\n⚠️  Mode: " + ("Production" if os.path.exists('./client_web') else "Simulation (client_web non trouvé)"))
    
    # Nettoyer les FIFOs au démarrage
    try:
        os.unlink('/tmp/fifo1')
        os.unlink('/tmp/fifo2')
    except:
        pass
    
    app.run(host='0.0.0.0', port=5000, debug=True)