-- Initialize database for Parfum Bazaar application

-- Create users table
CREATE TABLE IF NOT EXISTS PERFUME_USERS (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(20) NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Create maklers table
CREATE TABLE IF NOT EXISTS PERFUME_MAKLERS (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR(100) NOT NULL,
    address VARCHAR(200),
    birth_year INTEGER,
    user_id INTEGER,
    FOREIGN KEY (user_id) REFERENCES PERFUME_USERS(id)
);

-- Create goods table
CREATE TABLE IF NOT EXISTS PERFUME_GOODS (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR(100) NOT NULL,
    type VARCHAR(50) NOT NULL,
    unit_price DECIMAL(10,2) NOT NULL,
    supplier VARCHAR(100),
    expiry_date DATE,
    quantity INTEGER NOT NULL DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Create deals table
CREATE TABLE IF NOT EXISTS PERFUME_DEALS (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    deal_date DATETIME NOT NULL,
    good_name VARCHAR(100) NOT NULL,
    good_type VARCHAR(50) NOT NULL,
    quantity INTEGER NOT NULL,
    total_amount DECIMAL(12,2) NOT NULL,
    makler_id INTEGER NOT NULL,
    good_id INTEGER NOT NULL,
    buyer VARCHAR(100) NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (makler_id) REFERENCES PERFUME_MAKLERS(id),
    FOREIGN KEY (good_id) REFERENCES PERFUME_GOODS(id)
);

-- Create makler statistics table
CREATE TABLE IF NOT EXISTS PERFUME_MAKLERSTATS (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    makler_id INTEGER NOT NULL,
    good_name VARCHAR(100) NOT NULL,
    good_type VARCHAR(50) NOT NULL,
    total_quantity INTEGER NOT NULL DEFAULT 0,
    total_amount DECIMAL(12,2) NOT NULL DEFAULT 0,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (makler_id) REFERENCES PERFUME_MAKLERS(id),
    UNIQUE(makler_id, good_name, good_type)
);

-- Create indexes for performance
CREATE INDEX IF NOT EXISTS idx_users_username ON PERFUME_USERS(username);
CREATE INDEX IF NOT EXISTS idx_maklers_user_id ON PERFUME_MAKLERS(user_id);
CREATE INDEX IF NOT EXISTS idx_goods_name ON PERFUME_GOODS(name);
CREATE INDEX IF NOT EXISTS idx_deals_date ON PERFUME_DEALS(deal_date);
CREATE INDEX IF NOT EXISTS idx_deals_makler ON PERFUME_DEALS(makler_id);
CREATE INDEX IF NOT EXISTS idx_stats_makler ON PERFUME_MAKLERSTATS(makler_id);
