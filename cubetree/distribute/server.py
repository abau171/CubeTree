import socket
import threading

import cubetree.cube
import cubetree.distribute.json_socket_proxy
import cubetree.distribute.job_manager


class WorkerConnectionThread(threading.Thread):

    def __init__(self, connection, job_manager):
        super().__init__(daemon=True)
        self.connection = connection
        self.job_manager = job_manager

    def job_loop(self):
        while True:
            job = self.job_manager.get()
            try:
                self.connection.write(job)
                solution = self.connection.read()
            except cubetree.distribute.json_socket_proxy.EndOfStream:
                self.job_manager.return_job(job)
                return
            except OSError as e:
                self.job_manager.return_job(job)
                return
            if solution is not None:
                self.job_manager.set_solution(solution)
            self.job_manager.job_done()

    def run(self):
        self.job_loop()
        self.connection.close()


class WorkerListenerThread(threading.Thread):

    def __init__(self, hostname, port, job_manager):
        super().__init__(daemon=True)
        self.hostname = hostname
        self.port = port
        self.job_manager = job_manager

    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
            server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server_socket.bind((self.hostname, self.port))
            server_socket.listen(5)
            while True:
                client_socket, address = server_socket.accept()
                worker_connection = WorkerConnectionThread(cubetree.distribute.json_socket_proxy.JSONSocketProxy(client_socket), self.job_manager)
                worker_connection.start()


def gen_jobs(cube, depth, partial_solution=cubetree.cube.Algorithm()):
    if depth > 14:
        for face_id in range(6):
            face = cubetree.cube.Face(face_id)
            last_face = None if len(partial_solution.move_list) == 0 else partial_solution.move_list[-1][0]
            if last_face is not None:
                if face is last_face or (face.value < 3 and face is last_face.opposite()):
                    continue
            for turn_type_id in range(1, 4):
                turn_type = cubetree.cube.TurnType(turn_type_id)
                clone_cube = cubetree.cube.Cube(cube.get_state())
                clone_cube.turn(face, turn_type)
                yield from gen_jobs(clone_cube, depth - 1, partial_solution + cubetree.cube.Algorithm([(cubetree.cube.Face(face_id), cubetree.cube.TurnType(turn_type_id))]))
    else:
        yield cubetree.distribute.job_manager.Job(cube, depth, partial_solution)


class DistributedSolver:

    def __init__(self, hostname, port):
        self.job_manager = cubetree.distribute.job_manager.JobManager()
        WorkerListenerThread(hostname, port, self.job_manager).start()

    def solve(self, cube):
        if cube.is_solved():
            return cubetree.cube.Algorithm()
        for cur_depth in range(1, 21):
            print("DEPTH", cur_depth)
            self.job_manager.set_solution(None)
            self.job_manager.set_job_source(gen_jobs(cube, cur_depth))
            self.job_manager.join()
            solution = self.job_manager.get_solution()
            if solution is not None:
                return solution
