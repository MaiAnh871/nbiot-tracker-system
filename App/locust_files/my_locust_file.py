from locust import HttpUser, TaskSet, task

class LoginStep(TaskSet):
    @task
    def login(self):
        self.client.post("/login", {
            'email': 'LocustUser1@gmail.com', 'password': '123456'
        })

class LoginTest(HttpUser):
    tasks = [LoginStep]
    host = "http://blazedemo.com"
    sock = None

    def __init__(self):
        super(LoginTest, self).__init__()