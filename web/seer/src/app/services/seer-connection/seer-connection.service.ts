import { Injectable } from '@angular/core';
import { Connection } from './connection/connection';
import { Observable } from "rxjs/Rx";

@Injectable()
export class SeerConnectionService {
  private connections: Map<string, Connection> = new Map<string, Connection>();
  constructor() { }

  public connect(url: string): Observable<any> {
    if (this.connections.has(url)) {
      return this.connections.get(url).messages;
    } else {
      const connection = new Connection(url);
      this.connections.set(url, connection);
      return connection.messages;
    }
  }  

  public send(url: string, message: any): void {
     if (this.connections.has(url) === false) {
      throw new Error(`No connection by the url: ${url} exists, please call connect first`);
    }
    this.connections.get(url).send(message);
  }
}
