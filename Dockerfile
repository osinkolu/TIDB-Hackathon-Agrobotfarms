# Use an Ubuntu base image
FROM ubuntu:20.04

# Set the working directory in the container
WORKDIR /app

# Install necessary packages
RUN apt-get update \
    && apt-get install -y default-libmysqlclient-dev \
    && apt-get install -y python3 python3-pip python3-dev \
    && apt-get install -y python-mysqldb \
    && rm -rf /var/lib/apt/lists/*

# Copy the requirements file to the working directory
COPY requirements.txt .

# Install the required Python packages
RUN pip3 install --no-cache-dir -r requirements.txt

# Copy the Flask app code to the working directory
COPY app.py .

# Set the environment variable
ENV FLASK_APP=app.py

# Expose the port the Flask app will run on
EXPOSE 5000

# Run the Flask app when the container starts
CMD ["flask", "run", "--host=0.0.0.0"]
