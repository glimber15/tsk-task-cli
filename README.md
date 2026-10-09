# Todo CLI
A todo app which lives im your project. It's kinda like your TODO.md but more cli-y and only show you whats is important.

## Usage
| Command | What it does |
|---------|--------------|
| `tsk init` | Initialize tsk in project |
| `tsk add 'task title'` | Add a task |
| `tsk desc id 'description'` | Add description to a task |
| `tsk show` | Show pending tasks |
| `tsk show -c` | Show completed tasks also |
| `tsk show -d` | Show tasks with extra details |
| `tsk tick id` | Complete/Toggle a task |
| `tsk edit id 'new title'` | Edit added task title |
| `tsk del id` | Delete a task |
| `tsk kaboom` | Uninitialize tsk/ remove .tsk/ |

> `tsk show >> todo.txt` can gives you a task file

## Installation
* Copy the repository
* Make `install.sh` executable using `chmod +x install.sh`
* `./install.sh`
* tsk is now usable in your projects

### Uninstall
* make `uninstall.sh` executable
* `./uninstall.sh`
