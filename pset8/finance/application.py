import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash
from datetime import datetime

from helpers import apology, login_required, lookup, usd, positive_number

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    # Query database for stock and cash in user's possession
    rows = db.execute("SELECT symbol, shares FROM stock WHERE person_id = :id", id=session["user_id"])
    user_cash = db.execute("SELECT cash FROM users WHERE id = :id", id=session["user_id"])[0]["cash"]

    # Prepare necessary information to display on homepage
    total_assets = user_cash
    for row in rows:
        stock_data = lookup(row["symbol"])
        stock_price = stock_data["price"]
        shares_value = row["shares"] * stock_price

        # Add information to dict
        row["name"] = stock_data["name"]
        row["price"] = usd(stock_price)
        row["total"] = usd(shares_value)

        # Calculate value of all assets
        total_assets += shares_value

    return render_template("index.html", stocks=rows, total_assets=usd(total_assets), user_cash=usd(user_cash))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    # User reached route via POST (submitting a form)
    if request.method == "POST":

        # Get user input
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        # Get data from API
        stock_data = lookup(symbol)

        # Ensure symbol was submitted and exists
        if not stock_data:
            return apology("must provide a valid share symbol", 400)

        # Ensure positive number was submitted
        shares, error_message = positive_number(shares)
        if error_message:
            return apology(f"{error_message} of shares", 400)

        # Ensure user possess enought cash
        user_cash = db.execute("SELECT cash FROM users WHERE id = :id" , id=session["user_id"])[0]["cash"]
        stock_price = stock_data["price"]
        if user_cash < shares * stock_price:
            return apology("you can't afford this", 400)

        # Perform transaction

        # Update user cash
        db.execute("UPDATE users SET cash = :cash WHERE id = :id", id=session["user_id"],
                   cash=(user_cash - shares * stock_price))

        # Query for user stock
        row = db.execute("SELECT * FROM stock WHERE person_id = :id AND symbol = :symbol",
                         id=session["user_id"], symbol=symbol)

        # Update user stock possession
        # User does not own such stocks yet
        if len(row) == 0:
            db.execute("INSERT INTO stock (person_id, symbol, shares) VALUES (:id, :symbol, :shares)",
                   id=session["user_id"], symbol=symbol, shares=shares)

        # User already possess such stocks
        else:
            db.execute("UPDATE stock SET shares = :shares WHERE id = :id", shares=(row[0]["shares"] + shares), id=row[0]["id"])

        # Update user history
        db.execute("INSERT INTO history (person_id, symbol, shares, price, date) VALUES (:id, :symbol, :shares, :price, :date)",
                   id=session["user_id"], symbol=symbol, shares=shares, price=stock_price, date=datetime.now())

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    # Query database for history
    rows = db.execute("SELECT * FROM history WHERE person_id = :id", id=session["user_id"])

    # Format stock price
    for row in rows:
        row["price"] = usd(row["price"])

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
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

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


@app.route("/add_cash", methods=["GET", "POST"])
@login_required
def add_cash():
    """Add additional cash to an account"""

     # User reached route via POST (submitting a form)
    if request.method == "POST":

        # Get user input
        cash = request.form.get("cash")

        # Ensure positive number was submitted
        cash, error_message = positive_number(cash)
        if error_message:
            return apology(error_message, 400)

        # Query for user current cash
        current_cash = db.execute("SELECT cash FROM users WHERE id = :id", id=session["user_id"])[0]["cash"]

        # Update user cash
        db.execute("UPDATE users SET cash = :cash WHERE id = :id", id=session["user_id"], cash=(current_cash + cash))

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET
    else:
        return render_template("add_cash.html")

@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    # User reached route via POST (submitting a form)
    if request.method == "POST":

        # Get user input
        symbol = request.form.get("symbol")

        # Get data from API
        stock_data = lookup(symbol)

        # Format response string
        if stock_data:
            price_info = f"A share of {stock_data['name']} ({stock_data['symbol']}) costs {usd(stock_data['price'])}."
        else:
            price_info = ""

        return render_template("quote.html", price_info=price_info)

    # User reached route via GET
    else:
        return render_template("quote.html")

@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # User reached route via POST (submitting a form)
    if request.method == "POST":

        # Get user input
        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        # Ensure username was submitted
        if not username:
            return apology("must provide username", 400)

        # Ensure password was submitted
        if not password:
            return apology("must provide password", 400)

        # Ensure repeat password was submitted
        if not confirmation:
            return apology("must confirm passowrd", 400)

        # Ensure password and repeat password was the same
        if not password == confirmation:
            return apology("passwords must be the same", 400)

        # Check if username exists
        rows = db.execute("SELECT username FROM users WHERE username = :username",
                          username=username)
        if len(rows) != 0:
            return apology("username already exists", 400)

        # Create new user
        db.execute("INSERT INTO users (username, hash) VALUES (:username, :hash)",
                   username=username, hash=generate_password_hash(password))

        # Redirect user to login form
        return redirect("/login")

    # User reached route via GET
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    # User reached route via POST (submitting a form)
    if request.method == "POST":

        # Get user input
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        # Ensure symbol was submitted
        if not symbol:
            return apology("must provide a stock symbol")

        # Get data from API
        stock_data = lookup(symbol)

        # Ensure stock symbol is valid
        if not stock_data:
            return apology("must provide a valid share symbol", 400)

        # Ensure positive number was submitted
        shares, error_message = positive_number(shares)
        if error_message:
            return apology(f"{error_message} of shares", 400)

        # Query database for user's stock
        row = db.execute("SELECT * FROM stock WHERE person_id = :id AND symbol = :symbol", id=session["user_id"], symbol=symbol)

        # Ensure user possess such stock
        if len(row) == 0:
            return apology(f"you do not own any shares of {symbol} stock")

        user_stock = row[0]

        # Ensure user possess enough shares
        if shares > user_stock["shares"]:
            return apology(f"you do not own that many shares of {symbol} stock")

        # Perform transaction

        # Query for user current cash
        user_cash = db.execute("SELECT cash FROM users WHERE id = :id", id=session["user_id"])[0]["cash"]

        # Update user cash
        db.execute("UPDATE users SET cash = :cash WHERE id = :id", id=session["user_id"], cash=(user_cash + shares * stock_data["price"]))

        # Update user stock possession
        if shares == row[0]["shares"]:
            db.execute("DELETE FROM stock WHERE id = :id", id=user_stock["id"])
        else:
            db.execute("UPDATE stock SET shares = :shares WHERE id = :id", id=user_stock["id"], shares=(user_stock["shares"] - shares))

        # Update user history
        db.execute("INSERT INTO history (person_id, symbol, shares, price, date) VALUES (:id, :symbol, :shares, :price, :date)",
                   id=session["user_id"], symbol=symbol, shares=-shares, price=stock_data["price"], date=datetime.now())

        # Redirect user to home page
        return redirect("/")

     # User reached route via GET
    else:
        rows = db.execute("SELECT symbol FROM stock WHERE person_id = :id", id=session["user_id"])
        return render_template("sell.html", symbols=rows)


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
