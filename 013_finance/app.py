import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.security import check_password_hash, generate_password_hash
import re

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# If not already created, create SQL table to keep track of current holdings
db.execute("""CREATE TABLE IF NOT EXISTS holdings (
            symbol TEXT PRIMARY KEY NOT NULL,
            name TEXT NOT NULL,
            quantity INTEGER NOT NULL,
            currency VARCHAR(3) NOT NULL,
            date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
            user_id INTEGER NOT NULL,
            FOREIGN KEY(user_id) REFERENCES users(id))""")

# If not already created, create SQL table to keep track of purchases
db.execute("""CREATE TABLE IF NOT EXISTS purchases (
            id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            user_id INTEGER NOT NULL,
            symbol TEXT NOT NULL,
            name TEXT NOT NULL,
            quantity INTEGER NOT NULL,
            price NUMERIC NOT NULL,
            currency VARCHAR(3) NOT NULL,
            date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(user_id) REFERENCES users(id))""")

# If not already created, create SQL table to keep track of sales
db.execute("""CREATE TABLE IF NOT EXISTS sales (
            id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            user_id INTEGER NOT NULL,
            symbol TEXT NOT NULL,
            name TEXT NOT NULL,
            quantity INTEGER NOT NULL,
            price NUMERIC NOT NULL,
            currency VARCHAR(3) NOT NULL,
            date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(user_id) REFERENCES users(id))""")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    # Get user holding data
    rows = db.execute("SELECT symbol, name, quantity, currency FROM holdings WHERE user_id = ?", session["user_id"])
    user_cash = db.execute("SELECT * FROM users WHERE id = ?", session["user_id"])[0]["cash"]

    # Total current stock prices
    total = user_cash

    # Update share price with current price
    for row in rows:
        price = lookup(row["symbol"])["price"]
        row["price"] = price
        total += price * row["quantity"]

    return render_template("index.html", holdings=rows, cash=user_cash, total=round(total, 2))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    if request.method == "POST":

        # Get stock symbol
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        # Ensure symbol was entered
        if not symbol:
            return apology("must provide stock symbol", 400)

        # Ensure stock symbol exists
        elif not lookup(symbol):
            return apology("must provide a valid stock symbol", 400)

        # Ensure shares were entered
        elif not shares:
            return apology("must provide number of shares", 400)

        # Ensure number of shares entered is a whole number
        elif not shares.isnumeric():
            return apology("must be a whole positive number", 400)

        # Get stock price and user cash
        share_info = lookup(symbol)
        share_price = share_info["price"]
        share_symbol = share_info["symbol"]
        share_name = share_info["name"]
        user_cash = db.execute("SELECT * FROM users WHERE id = ?", session["user_id"])[0]["cash"]

        # Update user cash
        updated_cash = round(user_cash - (float(share_price) * float(shares)), 2)
        db.execute("UPDATE users SET cash = ? WHERE id = ?", updated_cash, session["user_id"])

        # Ensure user has enough cash to purchase shares
        if share_price > user_cash:
            return apology("insufficient cash in account to buy these shares", 400)

        # Add purchase to database
        db.execute("INSERT INTO purchases (user_id, symbol, name, quantity, price, currency) VALUES(?, ?, ?, ?, ?, ?)",
                   session["user_id"], share_symbol, share_name, shares, share_price, "USD")

        # Current share quantity
        current_shares = db.execute("SELECT quantity FROM holdings WHERE user_id = ? AND symbol = ?",
                                    session["user_id"], share_symbol)
        try:
            new_shares = current_shares[0]["quantity"] + int(shares)
        except IndexError:
            new_shares = shares

        # Update or insert share symbol in holdings
        db.execute("INSERT OR REPLACE INTO holdings (symbol, user_id, name, quantity, currency) VALUES(?, ?, ?, ?, ?)",
                   share_symbol, session["user_id"], share_name, new_shares, "USD")

        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    # Get user holdings data
    rows = db.execute("""SELECT *
                        FROM (SELECT * FROM purchases
                        UNION ALL
                        SELECT * FROM sales)
                        WHERE user_id = ?""",
                      session["user_id"])

    return render_template("history.html", transactions=rows)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 400)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 400)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    if request.method == "POST":

        # Get stock symbol
        symbol = request.form.get("symbol")

        # Ensure symbol was entered
        if not symbol:
            return apology("must provide stock symbol", 400)

        # Ensure stock symbol exists
        elif not lookup(symbol):
            return apology("must provide a valid stock symbol", 400)

        # Get quote
        quote = lookup(symbol)
        return render_template("quoted.html", quote=quote)

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    # User reached route via POST (as by submitting a form via POST)

    re_letter = re.compile("[a-zA-Z]")
    re_number = re.compile("[0-9]")
    re_symbol = re.compile("""[-!$%^&*()_+|~=`{}\[\]:";'<>?,.\/]""")

    if request.method == "POST":

        # Define username and password
        username = request.form.get("username")
        password = request.form.get("password")
        password_check = request.form.get("confirmation")

        # Ensure username was entered
        if not username:
            return apology("must provide username", 400)

        # Ensure username is not already taken
        elif db.execute("SELECT * FROM users WHERE username = ?", username):
            return apology("This username is already taken", 400)

        # Ensure password contains at least one letter, number and symbol
        elif not re_letter.search(password) or not re_number.search(password) or not re_symbol.search(password):
            return apology("password must contain at least one letter, number and symbol", 400)

        # Ensure password was entered and both entries match
        elif not password:
            return apology("must provide password", 400)

        elif not password_check:
            return apology("must confirm password", 400)

        elif password != password_check:
            return apology("passwords do not match", 400)

        # Add new user to database
        db.execute("INSERT INTO users (username, hash) VALUES(?, ?)", username, generate_password_hash(password))
        rows = db.execute("SELECT * FROM users WHERE username = ?", username)

        # Remember the user that created the account
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # If the user is not logged in, load register page
    elif not session:
        return render_template("register.html")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return redirect("/")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    if request.method == "POST":

        # Get stock symbol
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        # Get user holdings data of share symbol
        share_info = db.execute("SELECT * FROM holdings WHERE user_id = ? AND symbol = ?", session["user_id"], symbol)[0]

        # Ensure symbol selected is in client purchase history
        if symbol != share_info["symbol"]:
            return apology("must provide stock symbol", 400)

        # Ensure stock symbol exists
        elif not lookup(symbol):
            return apology("must provide a valid stock symbol", 400)

        # Ensure shares were entered
        elif not shares:
            return apology("must provide number of shares", 400)

        # Ensure number of shares entered is a whole number
        elif not shares.isnumeric():
            return apology("must be a whole positive number", 400)

        # Ensure user has enough shares owned to sell
        elif int(shares) > share_info["quantity"]:
            return apology("not enough shares owned", 400)

        # Update share price with current price
        share_price = lookup(share_info["symbol"])["price"]

        # Get stock symbol, name, and user cash
        share_symbol = share_info["symbol"]
        share_name = share_info["name"]
        user_cash = db.execute("SELECT * FROM users WHERE id = ?", session["user_id"])[0]["cash"]

        # Update user cash
        updated_cash = round(user_cash + (float(share_price) * float(shares)), 2)
        db.execute("UPDATE users SET cash = ? WHERE id = ?", updated_cash, session["user_id"])

        # Add sale to database
        db.execute("INSERT INTO sales (user_id, symbol, name, quantity, price, currency) VALUES(?, ?, ?, ?, ?, ?)",
                   session["user_id"], share_symbol, share_name, f"-{shares}", share_price, "USD")

        shares_left = share_info["quantity"] - int(shares)

        # Remove share symbol from holdings if no shares are left after sale
        if shares_left == 0:
            db.execute("DELETE FROM holdings WHERE symbol = ? AND user_id = ?",
                       share_symbol, session["user_id"])

        # Remove share symbol from holdings if no shares are left after sale
        else:
            db.execute("UPDATE holdings SET quantity = ? WHERE symbol = ? AND user_id = ?",
                       shares_left, share_symbol, session["user_id"])

        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        # Get user stock symbols
        rows = db.execute("SELECT symbol FROM purchases WHERE user_id = ? GROUP BY symbol;", session["user_id"])

        return render_template("sell.html", symbols=rows)