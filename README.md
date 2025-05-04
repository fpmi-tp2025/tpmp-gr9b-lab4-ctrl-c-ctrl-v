# Parphum Bazaar Management System

A console-based application for managing a perfume marketplace where wholesale buyers and sellers conduct business through intermediaries (maklers).

## Description

Parphum Bazaar Management System is a database-driven application that facilitates trading operations on a perfume marketplace. The system manages maklers (intermediaries), goods (perfumes and cosmetics), and sales transactions. It supports two user roles: administrators who manage the system and maklers who conduct sales. The application provides comprehensive reporting capabilities and tracks sales statistics for each makler.

## Features

- **User Authentication**: Role-based access control for administrators and maklers
- **Makler Management**: Add, view, and manage intermediary profiles
- **Inventory Control**: Manage perfume and cosmetic products with pricing and expiry tracking
- **Deal Processing**: Create and track sales transactions with automatic inventory updates
- **Reporting**: Generate sales reports by date, popular products, and supplier analysis
- **Statistics**: Track individual makler performance and sales metrics

## Installation

### Prerequisites

- GCC compiler
- SQLite3 development library
- Make build system

### Ubuntu/Debian Installation Steps

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential libsqlite3-dev sqlite3

# Clone the repository
git clone https://github.com/your-username/parfum-bazaar.git
cd parfum-bazaar

# Create required directories
mkdir -p build bin data

# Initialize database
make init_db

# Build the application
make all
```

## Usage

### Starting the Application

```bash
./bin/parfum_bazaar
```

### Default Credentials

**Administrator:**
- Username: `admin`
- Password: `admin123`

**Makler (Test Users):**
- Username: `makler1`, Password: `makler123`
- Username: `makler2`, Password: `makler123`

### Administrator Functions

1. Add new maklers to the system
2. Add products to inventory
3. View all sales transactions
4. Generate comprehensive reports
5. Monitor system-wide statistics

### Makler Functions

1. Create sales deals
2. View personal transaction history
3. Check personal sales statistics
4. Browse available products
5. Track personal performance metrics

### Running Tests

```bash
# Run all tests
make check

# Run individual test suites
make test_database
make test_auth
make test_deals

# Generate coverage report
make coverage
```

## Project Structure

```
parfum-bazaar/
├── src/            # Source files
│   ├── main.c
│   ├── auth.c
│   ├── database.c
│   ├── deals.c
│   ├── reports.c
│   └── ui.c
├── includes/       # Header files
├── test/           # Test files
├── data/           # Database initialization scripts
├── build/          # Compiled object files
└── bin/            # Executable binary
```

## Database Schema

The system uses SQLite database with the following main tables:
- `PERFUME_USERS`: System users (admin/makler)
- `PERFUME_MAKLERS`: Makler profiles
- `PERFUME_GOODS`: Product inventory
- `PERFUME_DEALS`: Sales transactions
- `PERFUME_MAKLERSTATS`: Aggregated sales statistics

## Contributing

This project was developed as part of a university laboratory assignment on mobile application programming.

### Team Members

- **Team Lead**: [Your Name] - Project architecture, database design, authentication system
- **Developer**: [Teammate Name] - UI implementation, reporting system, testing

Each team member contributed equally to:
- Requirements analysis and specification
- Database design and testing
- Code review and documentation
- CI/CD pipeline setup

## Development

### CI/CD

The project uses GitHub Actions for continuous integration:
- Automatic builds on push/pull requests
- Unit test execution
- Code coverage analysis

### Building from Source

```bash
# Clean build
make clean
make distclean

# Debug build
make debug

# Run valgrind for memory leaks
make valgrind
```

## License

This project is for educational purposes only.

## Acknowledgments

- Developed for laboratory work on Mobile Application Programming
- Based on database design principles and C programming practices
