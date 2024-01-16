import subprocess
import time
import os

def start_server():
    return subprocess.Popen(["./server"])

def connect_client():
    return subprocess.Popen(["./client"])

def send_message(client_process, message):
    message = message.encode() + b"\n"
    client_process.communicate(input=message)

def shutdown_client(client_process):
    client_process.terminate()
    client_process.wait()

def shutdown_server(server_process):
    server_process.terminate()
    server_process.wait()

def main():
    # Step 1: Start the server
    server_process = start_server()
    time.sleep(2)  # Allow the server to start

    # Step 2: Connect the client
    client_process = connect_client()
    time.sleep(2)  # Allow the client to connect

    # Step 3: Client sends a message
    send_message(client_process, "Thomas")
    time.sleep(2)
    send_message(client_process, "Hello, server!")
    current_time = time.localtime()
    header1 = str(current_time.tm_hour)+":"+str(current_time.tm_min)+":"+str(current_time.tm_sec)

    # Step 4: Second client sends a new message
    second_client_process = connect_client()
    time.sleep(2)  # Allow the second client to connect
    send_message(client_process, "Joris")
    time.sleep(2)
    send_message(second_client_process, "Hello, server!")
    current_time = time.localtime()
    header2 = str(current_time.tm_hour)+":"+str(current_time.tm_min)+":"+str(current_time.tm_sec)

    # Step 5: Client shutdown
    shutdown_client(client_process)
    time.sleep(2)  # Allow the client to close the connection

    # Step 6: Server shutdown
    shutdown_server(server_process)
    time.sleep(2)  # Allow the server to stop

    # Optionally, you can add assertions to check log contents, etc.
    for file in os.listdir("log_files/"):
        f = os.path.join("log_files/",file)
        with open(f, "r") as log_file:
            log_content = log_file.read()
            assert "[Thomas, 127.0.0.1: "+header1+"] Hello, server!" in log_content
            assert "[Joris, 127.0.0.1: "+header2+"] New message from the second client!" in log_content
            # Add more assertions as needed

if __name__ == "__main__":
    subprocess.run(["make"])
    main()
